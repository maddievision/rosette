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

#pragma once

#include <JuceHeader.h>
#include "rosette/playback.h"
#include "rosette/common.h"
#include "rosette/savedata.h"
#include "farbot/fifo.hpp"
#include "farbot/RealtimeObject.hpp"

//==============================================================================
/**
*/


// TODO(ruby): Temporary dummy sequence
struct RoseEvent {
    double t{};
    int note{};
    bool isOff{};
};


// TODO(ruby): Move this to farbot::RealtimeObject
struct RealTimeState {
    std::atomic<bool> isPlaying{};
    std::atomic<rosette::BPM> bpm{120};
    std::atomic<rosette::PPQ> ppq{};
    std::atomic<rosette::SampleRate> sampleRate{};
    std::atomic<std::size_t> bufferSize{};
    std::atomic<bool> hasCycle{};
    std::atomic<rosette::PPQ> cycleStart{};
    std::atomic<rosette::PPQ> cycleEnd{};
};

struct RosetteMidiMessage {
    rosette::PPQ ppq{};
    int status{};
    int byte1{};
    int byte2{};
};

using RosetteMidiMessageBuffer = farbot::fifo<RosetteMidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single>;
using JuceMidiMessageBuffer = farbot::fifo<juce::MidiMessage, farbot::fifo_options::concurrency::single, farbot::fifo_options::concurrency::single>;

class RosetteAudioProcessor  : public juce::AudioProcessor
{
    public:
    //==============================================================================
    RosetteAudioProcessor();
    ~RosetteAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    RealTimeState &getRealTimeState();
    rosette::PluginData &getPluginData();
    const rosette::PluginData &getPluginData() const;
    rosette::PluginCache &getPluginCache();
    const rosette::PluginCache &getPluginCache() const;
    std::shared_ptr<RosetteMidiMessageBuffer> getMidiMessageBuffer();
    std::shared_ptr<JuceMidiMessageBuffer> getMidiOutMessageBuffer();

    void makeUpdates();
    
    private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RosetteAudioProcessor)
    
    rosette::PluginData m_data{};
    rosette::PluginCache m_cache{};
    rosette::PlaybackData m_pbData{};
    rosette::PlaybackState m_pbState{};
    rosette::PlaybackMidiRenderState m_render{};
    
    std::shared_ptr<RosetteMidiMessageBuffer> m_midiMessageBuffer;
    std::shared_ptr<JuceMidiMessageBuffer> m_midiOutMessageBuffer;
    farbot::RealtimeObject<rosette::PlaybackEventList, farbot::RealtimeObjectOptions::nonRealtimeMutatable> m_playbackEventsRT;

    
    RealTimeState m_rtState{};
    
//    std::vector<RoseEvent> trackerEvents{};
//    juce::Optional<int> m_lastNote{};
//    int m_playerPos{};
//    double lastPosition{};
    
    void updateShadow();
    void updatePlaybackData();
    
    rosette::Sheet &getSheet();
    const rosette::Sheet &getSheet() const;
    rosette::Sheet &getShadowSheet();
    const rosette::Sheet &getShadowSheet() const;
        

    
    void setupDefaultState();
};
