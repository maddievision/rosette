/* Copyright 2026 Maddie Lim
 *
 * rosette is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rosette is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rosette.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RosetteAudioProcessor::RosetteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    setupDefaultState();
}

RosetteAudioProcessor::~RosetteAudioProcessor()
{
}

//==============================================================================
const juce::String RosetteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RosetteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RosetteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RosetteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RosetteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RosetteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RosetteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RosetteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RosetteAudioProcessor::getProgramName (int index)
{
    return {};
}

void RosetteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RosetteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RosetteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RosetteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RosetteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto sampleCount = buffer.getNumSamples();
    auto sampleRate = getSampleRate();
    m_rtState.bufferSize.store(sampleCount);
    m_rtState.sampleRate.store(sampleRate);
    buffer.clear();
    juce::MidiBuffer processedMidi;
    for (const auto metadata : midiMessages) {
        // passthrough any MIDI
        auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;
        processedMidi.addEvent(message, time);
        
        // debug, duplicate notes an octave higher
//        if (message.isNoteOnOrOff()) {
//            message.setNoteNumber(message.getNoteNumber() + 12);
//            processedMidi.addEvent(message, time);
//        }
    }
    
    const auto playhead = getPlayHead();
    const auto pos = playhead->getPosition();
    auto eventCount = trackerEvents.size();
    if (pos.hasValue()) {
        
        
        // realtime state
        m_rtState.isPlaying.store(pos->getIsPlaying());
        m_rtState.hasCycle.store(pos->getIsLooping());
        if (pos->getBpm().hasValue()) {
            m_rtState.bpm.store(*pos->getBpm());
        }
        if (pos->getPpqPosition().hasValue()) {
            m_rtState.ppq.store(*pos->getPpqPosition());
        }
        if (pos->getLoopPoints().hasValue()) {
            auto loopPoints = *pos->getLoopPoints();
            m_rtState.cycleStart.store(loopPoints.ppqStart);
            m_rtState.cycleEnd.store(loopPoints.ppqEnd);
        }

        
        
        if (!m_pbState.isPlaying && pos->getIsPlaying() && pos->getPpqPosition().hasValue()) {
            m_playerPos = 0;
            m_lastNote = {};
            // begin playback
            m_pbState.lastPPQ = *pos->getPpqPosition();
            m_pbState.isPlaying = true;
            m_pbState.needsResync = true;
       }
        
        if (!pos->getIsPlaying()) {
            m_pbState.isPlaying = false;
            m_playerPos = 0;
            m_pbState.lastPPQ = 0;
            if (m_lastNote.hasValue()) {
                // All notes off
                processedMidi.addEvent(juce::MidiMessage::controllerEvent(0, 123, 0), 0);
                m_lastNote = {};
            }
        }
        
        // attempt to detect a seek
        if (m_pbState.isPlaying && pos->getPpqPosition().hasValue() && pos->getBpm().hasValue()) {
            auto curPPQ = *pos->getPpqPosition();
            auto bpm = *pos->getBpm();
            auto spb = 60.0 / bpm;
            double secLength = static_cast<double>(sampleCount) / sampleRate;
            auto beatLength = secLength / spb;
            if (curPPQ < m_pbState.lastPPQ || curPPQ > m_pbState.lastPPQ + beatLength * 2) {
                m_pbState.needsResync = true;
                m_pbState.lastPPQ = curPPQ;
            }
        }
        
        if (m_pbState.isPlaying && m_pbState.needsResync) {
            m_pbState.needsResync = false;
            if (m_lastNote.hasValue()) {
                // All notes off
                processedMidi.addEvent(juce::MidiMessage::controllerEvent(0, 123, 0), 0);
                m_lastNote = {};
            }

            // skip event
            m_playerPos = 0;
            while (m_playerPos < eventCount && trackerEvents[m_playerPos].t < m_pbState.lastPPQ) {
                ++m_playerPos;
            }
        }
        
        
        if (m_pbState.isPlaying && pos->getPpqPosition().hasValue() && pos->getBpm().hasValue()) {
            m_pbState.lastPPQ = *pos->getPpqPosition();
            auto bpm = *pos->getBpm();
            auto spb = 60.0 / bpm;
            double secLength = static_cast<double>(sampleCount) / sampleRate;
            auto beatLength = secLength / spb;
            auto maxT = m_pbState.lastPPQ + beatLength;
            double cycleStart = 0;
            double cycleEnd = 0;
            double maxTAfterLoop = 0;
            double minTAfterLoop = 0;
            if (pos->getLoopPoints().hasValue()) {
                auto loopPoints = *pos->getLoopPoints();
                cycleStart = loopPoints.ppqStart;
                cycleEnd = loopPoints.ppqEnd;
            }
            bool processCycleLoop = false;
            
            // TODO(ruby): Note this is buggy!!! Especially if the host plays past the cycle
            if (pos->getIsLooping() && maxT >= cycleEnd && (cycleEnd - maxT < beatLength)) {
                // if it's past the sample buffer size we assume the host is playing past the cycle
                processCycleLoop = true;
                maxTAfterLoop = cycleStart + (maxT - cycleEnd);
                minTAfterLoop = cycleStart - (cycleEnd - m_pbState.lastPPQ);
            }

            while (m_playerPos < eventCount && (processCycleLoop ? trackerEvents[m_playerPos].t < cycleEnd : trackerEvents[m_playerPos].t <= maxT)) {
                const auto &ev = trackerEvents[m_playerPos];
                auto t = ev.t;
                auto sampleNum = (t - m_pbState.lastPPQ) * spb * sampleRate;
                if (m_lastNote.hasValue()) {
                    processedMidi.addEvent(juce::MidiMessage::noteOff(0, *m_lastNote), sampleNum);
                    m_lastNote = {};
                }
                if (!ev.isOff) {
                    auto m = juce::MidiMessage::noteOn(0, ev.note, static_cast<juce::uint8>(127));
                    processedMidi.addEvent(m, sampleNum);
                    m_lastNote = ev.note;
                }
                ++m_playerPos;
            }
            
            if (processCycleLoop) {
                m_playerPos = 0;
                while (m_playerPos < eventCount && trackerEvents[m_playerPos].t < cycleStart) {
                    ++m_playerPos;
                }

                processedMidi.addEvent(juce::MidiMessage::controllerEvent(0, 123, 0), (cycleEnd - m_pbState.lastPPQ) * spb * sampleRate);
                m_lastNote = {};

                while (m_playerPos < eventCount && trackerEvents[m_playerPos].t <= maxTAfterLoop) {
                    const auto &ev = trackerEvents[m_playerPos];
                    auto t = ev.t;
                    auto sampleNum = (t - minTAfterLoop) * spb * sampleRate;
                    if (m_lastNote.hasValue()) {
                        processedMidi.addEvent(juce::MidiMessage::noteOff(0, *m_lastNote), sampleNum);
                        m_lastNote = {};
                    }
                    if (!ev.isOff) {
                        auto m = juce::MidiMessage::noteOn(0, ev.note, static_cast<juce::uint8>(127));
                        processedMidi.addEvent(m, sampleNum);
                        m_lastNote = ev.note;
                    }
                    ++m_playerPos;
                }
                m_pbState.lastPPQ = minTAfterLoop;
            }
        }
    }
    
    midiMessages.swapWith(processedMidi);
}

//==============================================================================
bool RosetteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RosetteAudioProcessor::createEditor()
{
    return new RosetteAudioProcessorEditor (*this);
}

//==============================================================================
void RosetteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    m_data.saveState(destData);
}

void RosetteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryBlock readData(data, sizeInBytes);
    m_data.readState(readData);
    makeUpdates();
}

RealTimeState &RosetteAudioProcessor::getRealTimeState() {
    return m_rtState;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RosetteAudioProcessor();
}

rosette::PluginData &RosetteAudioProcessor::getPluginData() {
    return m_data;
}

const rosette::PluginData &RosetteAudioProcessor::getPluginData() const {
    return m_data;
}

rosette::PluginCache &RosetteAudioProcessor::getPluginCache() {
    return m_cache;
}

const rosette::PluginCache &RosetteAudioProcessor::getPluginCache() const {
    return m_cache;
}

void RosetteAudioProcessor::makeUpdates() {
    updateShadow();
    updatePlaybackData();
}

void RosetteAudioProcessor::updateShadow() {
    auto &sheet = getSheet();
    sheet.cacheAddresses();
    
    auto &shadow = getShadowSheet();
    
    // synchronise columns
    // remove extraneous cols
    std::set<rosette::ColAddress> colAddresses{};
    for (const auto &[addr, col]: shadow.columns) {
        if (!sheet.has(addr)) {
            colAddresses.insert(addr);
        }
    }
    for (const auto &addr : colAddresses) {
        shadow.columns.erase(addr);
    }
    
    // add missing columns
    colAddresses.clear();
    for (const auto &[addr, col]: sheet.columns) {
        if (!shadow.has(addr)) {
            colAddresses.insert(addr);
        }
    }
    
    for (const auto &addr : colAddresses) {
        shadow.getOrInsert(addr);
    }
    
    getShadowSheet().cacheAddresses();
    
    for (const auto &[addr, col]: sheet.columns) {
        auto &destCol = shadow[addr];
        destCol.events.clear();
        for (const auto &[t, ev]: col.events) {
            auto modColCount = sheet.modColumnCount(addr.channelIndex, addr.noteIndex);
            rosette::SheetEvent newEv = ev;
            auto &sd = newEv.shadowData;
            if (ev.type == rosette::EventType::Note) {
                // we try to determine length;
                auto it = col.events.upper_bound(t);
                if (it != col.events.end()) {
                    const auto &nextT = it->first;
                    sd.length = nextT - t;
                } else {
                    sd.length = -1; // indefinite
                }
                
                // we check if there's an attached volume mod;
                float volAmt = 1.0f;
                for (int i = 0; i < modColCount; ++i) {
                    auto modAddr = addr.getMod(addr.noteIndex, i);
                    if (!sheet.has(modAddr, t)) continue;
                    const auto &modEv = sheet.at(modAddr, t);
                    if (modEv.isEffectOfType(rosette::EffectType::Volume)) {
                        volAmt = modEv.param1 / 99.0f;
                    }
                }
                
                sd.volAmt = volAmt;
                
            }
            destCol.events.insert_or_assign(t, newEv);
        }
    }
}

void RosetteAudioProcessor::updatePlaybackData() {
    // render shadow to playback data
}

rosette::Sheet &RosetteAudioProcessor::getSheet() {
    return m_data.sheet;
}
const rosette::Sheet &RosetteAudioProcessor::getSheet() const {
    return m_data.sheet;
};
rosette::Sheet &RosetteAudioProcessor::getShadowSheet() {
    return m_cache.shadowSheet;
}
const rosette::Sheet &RosetteAudioProcessor::getShadowSheet() const {
    return m_cache.shadowSheet;
}

void RosetteAudioProcessor::setupDefaultState() {
    m_data.reset();
    makeUpdates();
    
    
    // dummy sequence for now
    trackerEvents.push_back({.t = 0.00, .note = 64});
    trackerEvents.push_back({.t = 0.25, .note = 64});
    trackerEvents.push_back({.t = 0.50, .isOff = true});
    trackerEvents.push_back({.t = 0.75, .note = 64});
    trackerEvents.push_back({.t = 1.00, .isOff = true});
    trackerEvents.push_back({.t = 1.25, .note = 60});
    trackerEvents.push_back({.t = 1.50, .note = 64});
    trackerEvents.push_back({.t = 1.75, .isOff = true});
    trackerEvents.push_back({.t = 2.00, .note = 67});
    trackerEvents.push_back({.t = 2.25, .isOff = true});
    trackerEvents.push_back({.t = 3.00, .note = 55});
    trackerEvents.push_back({.t = 3.25, .isOff = true});
}
