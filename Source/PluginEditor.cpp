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
#include "rosette/ui.h"
#include "rosette/presentation.h"
#include "rosette/drawing.h"

//==============================================================================
RosetteAudioProcessorEditor::RosetteAudioProcessorEditor (RosetteAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    auto &state = getState();
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    auto size = state.windowSize;
    setResizable(true, true);
    setResizeLimits(300, 300, 1048576, 1048576);
    setSize (size.getWidth(), size.getHeight());
    setWantsKeyboardFocus(true);
    setupDefaultState();
    juce::FontOptions editorFont {juce::Typeface::createSystemTypefaceFor(BinaryData::FantasqueSansMonoRegular_ttf, BinaryData::FantasqueSansMonoRegular_ttfSize)};
    m_font = new juce::Font(editorFont.withHeight(13));
    
    setupComponents();
    startTimerHz(60);
}

RosetteAudioProcessorEditor::~RosetteAudioProcessorEditor()
{
    delete(m_font);
}

void RosetteAudioProcessorEditor::setupComponents() {
    auto &state = getState();
    addAndMakeVisible(divisionSlider);
    divisionSlider.setRange(1, 192, 1);
    divisionSlider.setNumDecimalPlacesToDisplay(0);
    divisionSlider.setTextBoxIsEditable(true);
    divisionSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
    divisionSlider.setValue(state.division.den, juce::dontSendNotification);
    divisionSlider.onValueChange = [this] {
        auto &state = getState();
        state.division = rosette::rat(1, divisionSlider.getValue());
    };
    
    addAndMakeVisible(divisionLabel);
    divisionLabel.setText("Division", juce::dontSendNotification);
    divisionLabel.attachToComponent(&divisionSlider, true);
    
    
    addAndMakeVisible(stepSlider);
    stepSlider.setRange(1, 32, 1);
    stepSlider.setNumDecimalPlacesToDisplay(0);
    stepSlider.setTextBoxIsEditable(true);
    stepSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
    stepSlider.setValue(state.step, juce::dontSendNotification);
    stepSlider.onValueChange = [this] {
        auto &state = getState();
        state.step = stepSlider.getValue();
    };
    
    addAndMakeVisible(stepLabel);
    stepLabel.setText("Step", juce::dontSendNotification);
    stepLabel.attachToComponent(&stepSlider, true);
    
    addAndMakeVisible(octaveSlider);
    octaveSlider.setRange(0, 11, 1);
    octaveSlider.setNumDecimalPlacesToDisplay(0);
    octaveSlider.setTextBoxIsEditable(true);
    octaveSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
    octaveSlider.setValue(state.octave, juce::dontSendNotification);
    octaveSlider.onValueChange = [this] {
        auto &state = getState();
        state.octave = octaveSlider.getValue();
    };
    
    addAndMakeVisible(octaveLabel);
    octaveLabel.setText("Octave", juce::dontSendNotification);
    octaveLabel.attachToComponent(&octaveSlider, true);
    
    addAndMakeVisible(instrumentSlider);
    instrumentSlider.setRange(1, 128, 1);
    instrumentSlider.setNumDecimalPlacesToDisplay(0);
    instrumentSlider.setTextBoxIsEditable(true);
    instrumentSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
    instrumentSlider.setValue(state.instrument + 1, juce::dontSendNotification);
    instrumentSlider.onValueChange = [this] {
        auto &state = getState();
        state.instrument = instrumentSlider.getValue() - 1;
    };
    
    addAndMakeVisible(instrumentLabel);
    instrumentLabel.setText("Instr.", juce::dontSendNotification);
    instrumentLabel.attachToComponent(&instrumentSlider, true);
    
    addAndMakeVisible(enableStepInputButton);
    enableStepInputButton.setTitle("Step Input");
    enableStepInputButton.setButtonText("Step Input");
    enableStepInputButton.onClick = [this] {
        bool val = enableStepInputButton.getToggleState();
        getState().enableStepInput = val;
    };
    
    
    enableStepInputButton.setToggleState(state.enableStepInput, juce::dontSendNotification);
    
    addAndMakeVisible(enableMidiStepInputButton);
    enableMidiStepInputButton.setTitle("MIDI Input");
    enableMidiStepInputButton.setButtonText("MIDI Input");
    enableMidiStepInputButton.onClick = [this] {
        bool val = enableMidiStepInputButton.getToggleState();
        getState().enableMidiStepInput = val;
    };
    
    enableMidiStepInputButton.setToggleState(state.enableMidiStepInput, juce::dontSendNotification);
    
    addAndMakeVisible(enableMidiVelocityInputButton);
    enableMidiVelocityInputButton.setTitle("Use Velocity");
    enableMidiVelocityInputButton.setButtonText("Use Velocity");
    enableMidiVelocityInputButton.onClick = [this] {
        bool val = enableMidiVelocityInputButton.getToggleState();
        getState().enableMidiVelocityInput = val;
    };
    
    enableMidiVelocityInputButton.setToggleState(state.enableMidiVelocityInput, juce::dontSendNotification);
    
}

void RosetteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    getState().windowSize = juce::Rectangle<int>(getWidth(), getHeight());
    
    int x = rosette::ui_metrics::rulerX;
    int y = rosette::ui_metrics::headerY;
    int h = 32;
    enableStepInputButton.setBounds(x, y, 100, h);
    x += 100;
    enableMidiStepInputButton.setBounds(x, y, 100, h);
    enableMidiVelocityInputButton.setBounds(x, y + h, 100, h);
    x += 200;
    divisionSlider.setBounds(x, y, 80, h);
    x += 130;
    stepSlider.setBounds(x, y, 80, h);
    x += 130;
    octaveSlider.setBounds(x, y, 80, h);
    x += 130;
    instrumentSlider.setBounds(x, y, 80, h);
}

bool RosetteAudioProcessorEditor::keyPressed(const juce::KeyPress& key) {
    auto &state = getState();
    auto &config = getConfig();

    // TODO(ruby): Implement an action system, and key combo -> action map
    if (key == juce::KeyPress::downKey) {
        navigateTo(state.position.offsetRow(getStepLength()));
        return true;
    } else if (key == juce::KeyPress::upKey) {
        navigateTo(state.position.offsetRow(-getStepLength()));
        return true;
    } else if (key == juce::KeyPress::leftKey) {
        navigateTo(state.position.offsetCol(-1));
        return true;
    } else if (key == juce::KeyPress::rightKey) {
        navigateTo(state.position.offsetCol(1));
        return true;
    }
        
    if (state.enableStepInput) {
        auto c = key.getTextCharacter();
        auto addr = getCurrentAddress();

        if (addr.type == rosette::Scope::NOTE) {
            if (config.noteKeyMap.contains(c)) {
                insertEvent(rosette::SheetEvent::note(getNoteNumberInCurrentOctave(config.noteKeyMap[c]), getState().instrument), true, true);
                return true;
            } else if (c == '1') {
                insertEvent(rosette::SheetEvent::off(), true, true);
                return true;
            }
            
            if (c == '\\') {
                cycleNoteDisplayStyle();
                return true;
            }
        } else {
            if (c == '!') {
                insertEvent(rosette::SheetEvent::off());
                return true;
            }
        }
        
        if (addr.type == rosette::Scope::MOD) {
            if (config.charToEffectTypeMap.contains(c)) {
                auto effectType = config.charToEffectTypeMap[c];
                insertEvent(rosette::SheetEvent::effect(effectType, 0));
                return true;
            }
            
            if (config.quickEffectMap.contains(c)) {
                const auto &qe = config.quickEffectMap[c];
                insertEvent(rosette::SheetEvent::effect(qe.type, qe.param1, qe.param2));
                return true;
            }
        }
        
        if (key == juce::KeyPress::backspaceKey) {
            clearEvent();
            return true;
        } else if (key == juce::KeyPress::deleteKey) {
            deleteEvent();
            return true;
        }
    }
    
    return false;
}

//==============================================================================
void RosetteAudioProcessorEditor::paint (juce::Graphics& g)
{
    updatePlaybackStateCache();
    g.setFont(*m_font);
    drawBackground(g);
    drawPlayhead(g);
    drawGridLines(g);
    drawMarkers(g);
    drawSheet(g);
    drawCursor(g);
    drawHeader(g);
    drawRuler(g);
    drawCorner(g);
    drawRightMask(g);
    drawStatus(g);
    checkUpdateScroll();
}

void RosetteAudioProcessorEditor::drawBackground(juce::Graphics& g) {
    g.fillAll(rosette::ui_colours::bgColour);
}
void RosetteAudioProcessorEditor::drawGridLines(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    int h = bounds.getHeight();
    
    auto sheetPos = getSheetBasePos();
    int sX = sheetPos.getX();
    int sY = sheetPos.getY();

    // horizontal lines
    auto ppqHeight = getPPQHeight();
    auto timeBounds = getVisibleTimeArea();
    for (rosette::rat t = timeBounds.start; t <= timeBounds.end; t += getState().division) {
        if (t.den == 1) {
            g.setColour(rosette::ui_colours::gridlinePrimaryColour);
        } else {
            g.setColour(rosette::ui_colours::gridlineMinorColour);
        }
        int y = sY + t.toFloat() * ppqHeight;
        g.drawLine(0, y, w, y);
    }

    // vertical lines
    auto drawBounds = [&](const auto &boundsColl) {
        for (const auto &bounds : boundsColl) {
            int x1 = bounds.start + sX;
            int x2 = bounds.end + sX;
            g.drawLine(x1, 0, x1, h);
            g.drawLine(x2, 0, x2, h);
        }
    };
    
    const auto &dw = getDrawingCache();
    g.setColour(rosette::ui_colours::gridlineSecondaryColour);
    drawBounds(dw.columnBounds);
    g.setColour(rosette::ui_colours::gridlinePrimaryColour);
    drawBounds(dw.channelBounds);
}

void RosetteAudioProcessorEditor::drawMarkers(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();

    auto ppqHeight = getPPQHeight();
    const auto &pb = getPlaybackCache();

    auto sheetGlobalPos = getSheetBasePos(true);
    
    int gX = sheetGlobalPos.getX();
    int gY = sheetGlobalPos.getY();
    
    if (pb.hasCycle) {
        g.setColour(rosette::ui_colours::gridlineMarkerColour);
        int y1 = gY + pb.cycleStart * ppqHeight;
        int y2 = gY + pb.cycleEnd * ppqHeight;
        g.drawLine(0, y1, w, y1);
        g.drawLine(0, y2, w, y2);
        
        int iY = y1 + rosette::ui_metrics::minRowHeight;
        int iH = y2 - iY;
        
        rosette::drawing::drawEffect(g, "LP", 1.0f, rosette::ui_colours::noteMetaColour, rosette::drawing::NoteDrawFlags::None, gX, y1, rosette::ui_metrics::globalFxElWidth, rosette::ui_metrics::minRowHeight);
        rosette::drawing::drawEffect(g, "", 0.5f, rosette::ui_colours::noteMetaColour, rosette::drawing::NoteDrawFlags::None, gX, iY, rosette::ui_metrics::globalFxElWidth, iH);
        rosette::drawing::drawEffect(g, "END", 1.0f, rosette::ui_colours::noteMetaColour, rosette::drawing::NoteDrawFlags::None, gX, y2, rosette::ui_metrics::globalFxElWidth, rosette::ui_metrics::minRowHeight);
    }
}

void RosetteAudioProcessorEditor::drawRuler(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int h = bounds.getHeight();
    int w = bounds.getWidth();
    int rW = rosette::ui_metrics::rulerWidth;
    int rX = rosette::ui_metrics::rulerX;
    int ssY = rosette::ui_metrics::sheetY;

    g.setColour(rosette::ui_colours::rulerBgColour);
    g.fillRect(rX, ssY, rW, h - ssY);

    drawPlayhead(g, true);
    
    auto sheetPos = getSheetBasePos();
    int sY = sheetPos.getY();
    
    auto &pb = getPlaybackCache();
    auto &state = getState();
    
    rosette::rat cT = pb.isPlaying ? rosette::rat(pb.ppq * 960.0, 960).quantize(state.division.den) : state.position.row;
    
    // horizontal lines
    auto ppqHeight = getPPQHeight();
    auto timeBounds = getVisibleTimeArea();
    int dCount = 0;
    for (rosette::rat t = timeBounds.start; t <= timeBounds.end; t += getState().division) {
        int y = sY + t.toFloat() * ppqHeight;
        if (t == cT) {
            g.setColour(rosette::ui_colours::rulerCurrentTextColour);
            g.drawText(juce::String::formatted("%d", t.toWhole() + 1), rosette::ui_metrics::rulerBeatX, y, 30, 30, juce::Justification::topLeft);
        } else if (t.den == 1) {
            if (cT.toWhole() == t.toWhole()) {
                g.setColour(rosette::ui_colours::rulerCurrentTextColour);
            } else {
                g.setColour(rosette::ui_colours::rulerBeatTextColour);
            }
            g.drawText(juce::String::formatted("%d", t.num + 1), rosette::ui_metrics::rulerBeatX, y, 30, 30, juce::Justification::topLeft);
        }
        
        int dNum = dCount % state.division.den;
        if (t == cT) {
            g.setColour(rosette::ui_colours::rulerCurrentTextColour);
        } else {
            g.setColour(rosette::ui_colours::rulerDivisionTextColour);
        }
        g.drawText(juce::String::formatted("%d", dNum + 1), rosette::ui_metrics::rulerDivX, y, 30, 30, juce::Justification::topLeft);
        dCount++;
    }
}

void RosetteAudioProcessorEditor::drawStatus(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int lw = bounds.getWidth();
    int lh = bounds.getHeight();
    g.setColour(rosette::ui_colours::statusBgColour);
    int x = rosette::ui_metrics::statusX;
    int h = rosette::ui_metrics::statusHeight;
    int y = lh - h;

    g.fillRect(x, y, lw - x, h);
    
    const auto &pb = getPlaybackCache();
    auto isPlaying = pb.isPlaying;
    auto bpm = pb.bpm;
    auto ppq = pb.ppq;
    auto sr = pb.sampleRate;
    auto bufferSize = pb.bufferSize;
    
    g.setColour(rosette::ui_colours::eventTextColour);
    if (isPlaying) {
        auto str = juce::String::formatted("[Playing] Beat: %d, BPM: %d, Sample Rate: %d, Buffer Size: %d", static_cast<int>(ppq) + 1, static_cast<int>(bpm), static_cast<int>(sr), static_cast<int>(bufferSize));
        g.drawText(str, x + rosette::ui_metrics::textInsetX, y, lw - x, h, juce::Justification::centred);
    } else {
        auto str = juce::String::formatted("[Stopped] Beat: %d, BPM: %d, Sample Rate: %d, Buffer Size: %d", static_cast<int>(ppq) + 1, static_cast<int>(bpm), static_cast<int>(sr), static_cast<int>(bufferSize));
        g.drawText(str, x + rosette::ui_metrics::textInsetX, y, lw - x, h, juce::Justification::centred);
    }
    
}
void RosetteAudioProcessorEditor::drawHeader(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    g.setColour(rosette::ui_colours::channelHeaderBgColour);
    int x = rosette::ui_metrics::headerX;
    int y = rosette::ui_metrics::headerY;
    int h = rosette::ui_metrics::headerHeight;
    g.fillRect(x, y, w - x, h);
}
void RosetteAudioProcessorEditor::drawCorner(juce::Graphics& g) {
    g.setColour(rosette::ui_colours::channelHeaderBgColour);
    int x = rosette::ui_metrics::cornerHeaderX;
    int y = rosette::ui_metrics::cornerHeaderY;
    int w = rosette::ui_metrics::cornerWidth;
    int h = rosette::ui_metrics::cornerHeight;
    g.fillRect(x, y, w, h);
}
void RosetteAudioProcessorEditor::drawSheet(juce::Graphics& g) {
    const auto &sheet = getShadowSheet();
    auto s = getSheetBasePos();
    int sX = s.getX();
    int sY = s.getY();
    int colIndex = 0;
    auto ppqHeight = getPPQHeight();
    auto timeBounds = getVisibleTimeArea();
    
    const auto &dw = getDrawingCache();
    const auto &pb = getPlaybackCache();
    const auto &cfgc = getConfigCache();
    for (const auto &[addr, col] : sheet.columns) {
        const auto &bounds = dw.columnBounds[colIndex];
        int cX = sX + bounds.start;
        int cW = bounds.length();
        for (const auto &[t, ev] : col.events) {
            int eY = sY + t.toFloat() * ppqHeight;
            rosette::rat eLen{};
            if (ev.shadowData.length.isNegative()) {
                eLen = timeBounds.end - t;
            } else {
                eLen = std::max(getState().division, ev.shadowData.length);
            }
            int flags{};
            if (pb.isPlaying && ev.type == rosette::EventType::Note) {
                if (pb.ppq >= t.toFloat() && pb.ppq < (t + eLen).toFloat()) {
                    flags |= rosette::drawing::NoteDrawFlags::Active;
                } else {
                    flags |= rosette::drawing::NoteDrawFlags::Inactive;
                }
            }
            int eH = eLen.toFloat() * ppqHeight;
            int eW = cW - 2; // TODO(ruby): Magic number!
            if (!t.divisibleBy(getState().division.den)) {
                flags |= rosette::drawing::NoteDrawFlags::OffGrid;
            }
            
            switch (ev.type) {
                case rosette::EventType::Off:
                    rosette::drawing::drawOff(g, flags, cX, eY, eW, eH);
                    break;
                case rosette::EventType::Note:
                    rosette::drawing::drawNote(g, ev.noteNumber, ev.instrument, ev.noteDisplayStyle, ev.shadowData.volAmt, flags, cX, eY, eW, eH);
                    break;
                case rosette::EventType::Effect:
                    char prefix = '?';
                    if (cfgc.effectTypeToCharMap.contains(ev.effectType)) {
                        prefix = cfgc.effectTypeToCharMap.at(ev.effectType);
                    }
                    juce::String label = juce::String::formatted("%c%02d", prefix, ev.param1);
                    rosette::drawing::drawEffect(g, label, 1.0, rosette::ui_colours::noteVolColour, flags, cX, eY, eW, eH);
                    break;
            }
        }
        ++colIndex;
    }
}


void RosetteAudioProcessorEditor::drawPlayhead(juce::Graphics& g, bool rulerWidth) {
    int w = rulerWidth ? rosette::ui_metrics::rulerWidth : getLocalBounds().getWidth();
    int pY{};
    const auto &pb = getPlaybackCache();
    if (!pb.isPlaying) {
        pY = getCursorRect().getY();
        g.setColour(rosette::ui_colours::currentRowColour);
    } else {
        pY = getPlaybackY();
        g.setColour(rosette::ui_colours::playTimeColour);
    }
    g.fillRect(0, pY, w, static_cast<int>(rosette::ui_metrics::minRowHeight));
}

void RosetteAudioProcessorEditor::drawCursor(juce::Graphics& g) {
    const auto &pb = getPlaybackCache();
    if (pb.isPlaying) return;
    auto rect = getCursorRect();
    g.setColour(rosette::ui_colours::cursorOutlineColour);
    g.drawRect(rect);
}

void RosetteAudioProcessorEditor::drawRightMask(juce::Graphics &g) {
    const auto &dw = getDrawingCache();
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    int h = bounds.getHeight();
    auto s = getSheetBasePos(false, false);
    int sY = s.getY();
    auto ss = getSheetBasePos();
    int sX = ss.getX();
    int colEnd = sX + dw.channelBounds.back().end;


    g.setColour(rosette::ui_colours::bgColour);
    g.fillRect(colEnd, sY, w - colEnd, h - sY);
}

void RosetteAudioProcessorEditor::timerCallback() {
    auto midiBuffer = getProcessor()->getMidiMessageBuffer();
    RosetteMidiMessage msg;
    while (midiBuffer->pop(msg)) {
        handleMidiMessage(msg);
    }
    repaint();
}

void RosetteAudioProcessorEditor::handleMidiMessage(const RosetteMidiMessage &msg) {
    auto status = msg.status >> 4;
    if (status == 0x9 && msg.byte2 > 0) {
        auto &state = getState();
        if (state.enableStepInput && state.enableMidiStepInput) {
            const auto &t = state.position.row;
            juce::Optional<float> vol{};
            if (state.enableMidiVelocityInput) {
                vol = msg.byte2 / 127.0f;
            }
            insertEvent(rosette::SheetEvent::note(msg.byte1, state.instrument), true, false, vol);
        }
    }
}


rosette::Sheet &RosetteAudioProcessorEditor::getSheet() {
    return getPluginData().sheet;
}
const rosette::Sheet &RosetteAudioProcessorEditor::getSheet() const {
    return getPluginData().sheet;
}
rosette::Sheet &RosetteAudioProcessorEditor::getShadowSheet() {
    return getPluginCache().shadowSheet;
}
const rosette::Sheet &RosetteAudioProcessorEditor::getShadowSheet() const {
    return getPluginCache().shadowSheet;
}

void RosetteAudioProcessorEditor::makeUpdates() {
    auto proc = getProcessor();
    proc->makeUpdates();
    updateEditorCache();
}

void RosetteAudioProcessorEditor::updateEditorCache() {
    auto &ceMap = getConfig().charToEffectTypeMap;
    auto &ecMap = getConfigCache().effectTypeToCharMap;
    ecMap.clear();
    for (auto [c, e] : ceMap) {
        ecMap.insert_or_assign(e, c);
    }

    const auto &sheet = getShadowSheet();
    const auto &columns = sheet.getColumns();
    auto &dw = getDrawingCache();
    dw.columnBounds.clear();
    dw.channelBounds.clear();
    
    rosette::ColumnIndex i = 0;
    juce::Optional<rosette::ChannelIndex> lastC{};
    int x = 0;
    int channelX = 0;
    
    auto saveChannelWidth = [&] {
        if (lastC.hasValue()) {
            x += rosette::ui_metrics::columnGap;
            dw.channelBounds.push_back({.start = channelX, .end = x});
        }
    };
    
    for (const auto &col : columns) {
        if (!lastC.hasValue() || *lastC != col.channelIndex) {
            // new channel
            saveChannelWidth();
            channelX = x;
            lastC = col.channelIndex;
        }
        
        int w = 0;
        switch (col.type) {
            case rosette::Scope::NOTE:
                w = rosette::ui_metrics::noteColWidth;
                break;
            case rosette::Scope::MOD:
                w = rosette::ui_metrics::modColWidth;
                break;
            case rosette::Scope::CHANNEL:
                w = rosette::ui_metrics::channelFxColWidth;
                break;
        }
        dw.columnBounds.push_back({.start = x, .end = x + w});
        x += w;
        ++i;
    }
    saveChannelWidth();
}

float RosetteAudioProcessorEditor::getPPQHeight() const {
    return getState().division.den * rosette::ui_metrics::minRowHeight;
}

rosette::bounds<rosette::rat> RosetteAudioProcessorEditor::getVisibleTimeArea() const {
    auto bounds = getLocalBounds();
    auto h = bounds.getHeight();
    const auto &temp = getTemp();
    int yStart = temp.scrollPos.y;
    int yEnd = temp.scrollPos.y + h;
    float ppqHeight = getPPQHeight();

    rosette::rat tStart = rosette::rat(std::floor(yStart / ppqHeight));
    rosette::rat tEnd = rosette::rat(std::ceil(yEnd / ppqHeight));
    
    if (tStart.isNegative()) {
        tStart = rosette::rat{0};
    }
    
    return rosette::bounds{
        .start = tStart,
        .end = tEnd
    };
}

void RosetteAudioProcessorEditor::updatePlaybackStateCache() {
    const auto &rt = getProcessor()->getRealTimeState();
    auto &pb = getPlaybackCache();
    pb.isPlaying = rt.isPlaying.load();
    pb.bpm = rt.bpm.load();
    pb.ppq = rt.ppq.load();
    pb.sampleRate = rt.sampleRate.load();
    pb.bufferSize = rt.bufferSize.load();
    pb.hasCycle = rt.hasCycle.load();
    pb.cycleStart = rt.cycleStart.load();
    pb.cycleEnd = rt.cycleEnd.load();
    
    if (pb.wasPlaying != pb.isPlaying) {
        playbackStateChanged(pb.isPlaying);
        pb.wasPlaying = pb.isPlaying;
    }
}

juce::Point<int> RosetteAudioProcessorEditor::getSheetBasePos(bool forGlobal, bool applyScroll) const {
    int sX = forGlobal ? rosette::ui_metrics::sheetX : rosette::ui_metrics::sheetChanStartX;
    int sY = rosette::ui_metrics::sheetY;
    
    const auto &temp = getTemp();
    
    if (applyScroll) {
        return juce::Point<int>(sX, sY) - temp.scrollPos;
    }
    return juce::Point<int>(sX, sY);
}

double RosetteAudioProcessorEditor::getCurrentTime() const {
    const auto &pb = getPlaybackCache();
    const auto &state = getState();
    if (pb.isPlaying) {
        return pb.ppq;
    } else {
        return state.position.row.toFloat();
    }
}

rosette::rat RosetteAudioProcessorEditor::getStepLength() const {
    const auto &state = getState();
    return state.division * state.step;
}

void RosetteAudioProcessorEditor::navigateTo(const rosette::SheetPoint& position) {
    auto &state = getState();
    auto &pCache = getPluginCache();
    rosette::SheetPoint newPos = position;
    newPos.col = std::clamp(newPos.col, 0, static_cast<int>(pCache.shadowSheet.columnCount()) - 1);
    newPos.row = std::max(rosette::rat{}, newPos.row);
    state.position = newPos;
    auto &temp = getTemp();
    temp.shouldUpdateScroll = true;
}

void RosetteAudioProcessorEditor::checkUpdateScroll() {
    auto &temp = getTemp();
    auto &pb = getPlaybackCache();
    if ((pb.isPlaying || temp.shouldUpdateScroll) && !temp.mouseGrab.active) {
        temp.shouldUpdateScroll = false;
        
        auto bounds = getLocalBounds();
        int w = bounds.getWidth();
        int h = bounds.getHeight();
        bool checkX{};
        int cY{};
        int cX{};
        
        if (pb.isPlaying) {
            cY = getPlaybackY(false);
        } else {
            checkX = true;
            auto rect = getCursorRect(false);
            cX = rect.getX();
            cY = rect.getY();
        }
        
        int scrX = temp.scrollPos.getX();
        int scrY = temp.scrollPos.getY();
        
        if (checkX) {
            if (cX - scrX < rosette::ui_metrics::scrollThresholdX) {
                temp.scrollPos.setX(cX - rosette::ui_metrics::scrollThresholdX);
                if (temp.scrollPos.getX() < 0) temp.scrollPos.setX(0);
            } else if (cX - scrX
                       > w - rosette::ui_metrics::scrollThresholdX - rosette::ui_metrics::minimapWidth) {
                temp.scrollPos.x = cX - w + rosette::ui_metrics::scrollThresholdX + rosette::ui_metrics::minimapWidth;
            }
        }
        
        int topScrollThresholdY = (pb.isPlaying ? (rosette::ui_metrics::scrollThresholdNpY * static_cast<float>(h)) : rosette::ui_metrics::scrollThresholdY);
        int bottomScrollThresholdY = (pb.isPlaying ? (rosette::ui_metrics::scrollThresholdNpY * static_cast<float>(h)) : rosette::ui_metrics::scrollThresholdY);
        
        if (cY - scrY < topScrollThresholdY) {
            temp.scrollPos.setY(cY - topScrollThresholdY);
            if (temp.scrollPos.getY() < 0) temp.scrollPos.setY(0);
        } else if (cY - scrY > h - bottomScrollThresholdY) {
            temp.scrollPos.setY(cY - h + bottomScrollThresholdY);
        }
    }
}

juce::Rectangle<int> RosetteAudioProcessorEditor::getCursorRect(bool applyScroll) {
    auto s = getSheetBasePos(false, applyScroll);
    auto ppqHeight = getPPQHeight();
    int sX = s.getX();
    int sY = s.getY();
    const auto &dw = getDrawingCache();
    const auto &state = getState();
    const auto &bounds = dw.columnBounds[getState().position.col];
    int cY = sY + state.position.row.toFloat() * ppqHeight;
    int cX = sX + bounds.start;
    int cW = bounds.length();
    int cH = rosette::ui_metrics::minRowHeight;
    return juce::Rectangle<int>(cX, cY, cW, cH);
}

int RosetteAudioProcessorEditor::getPlaybackY(bool applyScroll) {
    auto s = getSheetBasePos(false, applyScroll);
    auto ppqHeight = getPPQHeight();
    const auto &pb = getPlaybackCache();
    return s.getY() + pb.ppq * ppqHeight;
}

void RosetteAudioProcessorEditor::insertEvent(const rosette::SheetEvent &event, bool advance, bool monitor, juce::Optional<float> withVolume) {
    const auto &state = getState();
    auto &sheet = getSheet();
    auto addr = getCurrentAddress();
    auto &col = sheet[addr];
    const auto &t = state.position.row;
    
    col.events.insert_or_assign(t, event);
    
    if (event.type == rosette::EventType::Note && withVolume.hasValue()) {
        auto modAddr = addr.getMod(addr.noteIndex, 0);
        if (sheet.has(modAddr)) {
            auto &modCol = sheet[modAddr];
            modCol.events.insert_or_assign(t, rosette::SheetEvent::effect(rosette::EffectType::Volume, *withVolume * 99.0f));
        }
    }
    
    makeUpdates();
    
    if (advance) {
        advanceToNextStep();
    }
    
    if (monitor) {
        auto &temp = getTemp();
        if ((event.type == rosette::EventType::Off || event.type == rosette::EventType::Note) && temp.lastMonitoredNote.hasValue()) {
            auto note = *temp.lastMonitoredNote;
            queueMidiMessage(juce::MidiMessage::noteOff(0, note));
            temp.lastMonitoredNote = {};
        }
        
        if (event.type == rosette::EventType::Note) {
            queueMidiMessage(juce::MidiMessage::noteOn(0, event.noteNumber, 1.0f));
            temp.lastMonitoredNote = event.noteNumber;
        }
    }
}

void RosetteAudioProcessorEditor::cycleNoteDisplayStyle() {
    const auto &state = getState();
    auto &sheet = getSheet();
    auto &col = sheet.columnAtIndex(state.position.col);
    const auto &t = state.position.row;
    
    if (col.has(t)) {
        auto &ev = col[t];
        if (ev.type != rosette::EventType::Note) {
            return;
        }
        
        // TODO(ruby): We actually need to determine how many available styles there are per note
        ev.noteDisplayStyle = (ev.noteDisplayStyle + 1) % 4;
        makeUpdates();
    }

}

rosette::NoteNumber RosetteAudioProcessorEditor::getNoteNumberInCurrentOctave(int baseNote) const {
    return baseNote + getState().octave * 12;
}

void RosetteAudioProcessorEditor::clearEvent(bool advance) {
    auto &sheet = getSheet();
    const auto &state = getState();
    auto &col = sheet.columnAtIndex(state.position.col);
    const auto &t = state.position.row;

    if (col.has(t)) {
        col.events.erase(t);
        makeUpdates();
    }
    if (advance) {
        advanceToNextStep();
    }
}

void RosetteAudioProcessorEditor::deleteEvent() {
    // delete everything up until the next step then shift up one step
}

void RosetteAudioProcessorEditor::advanceToNextStep() {
    navigateTo(getState().position.offsetRow(getStepLength()));
}

rosette::Column& RosetteAudioProcessorEditor::getCurrentColumn(bool inShadow) {
    auto &sheet = inShadow ? getShadowSheet() : getSheet();
    return sheet[getState().position.col];
}

rosette::ColAddress RosetteAudioProcessorEditor::getCurrentAddress() const {
    const auto &sheet = getSheet();
    return sheet.getAddress(getState().position.col);
}

rosette::PluginData &RosetteAudioProcessorEditor::getPluginData() {
    auto proc = getProcessor();
    return proc->getPluginData();
}

const rosette::PluginData &RosetteAudioProcessorEditor::getPluginData() const {
    auto proc = getProcessor();
    return proc->getPluginData();
}

rosette::PluginCache &RosetteAudioProcessorEditor::getPluginCache() {
    auto proc = getProcessor();
    return proc->getPluginCache();
}

const rosette::PluginCache &RosetteAudioProcessorEditor::getPluginCache() const {
    auto proc = getProcessor();
    return proc->getPluginCache();
}

rosette::PluginEditorState &RosetteAudioProcessorEditor::getState() {
    return getPluginData().editorState;
}
const rosette::PluginEditorState &RosetteAudioProcessorEditor::getState() const {
    return getPluginData().editorState;
}
rosette::PluginEditorConfig &RosetteAudioProcessorEditor::getConfig() {
    return getPluginData().config;
}
const rosette::PluginEditorConfig &RosetteAudioProcessorEditor::getConfig() const {
    return getPluginData().config;
}

rosette::EditorCache &RosetteAudioProcessorEditor::getCache() {
    return m_cache;
}
const rosette::EditorCache &RosetteAudioProcessorEditor::getCache() const {
    return m_cache;
}

rosette::PlaybackStateCache &RosetteAudioProcessorEditor::getPlaybackCache() {
    return getCache().playback;
}
const rosette::PlaybackStateCache &RosetteAudioProcessorEditor::getPlaybackCache() const {
    return getCache().playback;
}

rosette::DrawingCache &RosetteAudioProcessorEditor::getDrawingCache() {
    return getCache().drawing;
}
const rosette::DrawingCache &RosetteAudioProcessorEditor::getDrawingCache() const {
    return getCache().drawing;
}

rosette::EditorConfigCache &RosetteAudioProcessorEditor::getConfigCache() {
    return getCache().config;
}
const rosette::EditorConfigCache &RosetteAudioProcessorEditor::getConfigCache() const {
    return getCache().config;
}

rosette::EditorTempState &RosetteAudioProcessorEditor::getTemp() {
    return getCache().temp;
}
const rosette::EditorTempState &RosetteAudioProcessorEditor::getTemp() const {
    return getCache().temp;
}

RosetteAudioProcessor* RosetteAudioProcessorEditor::getProcessor() {
    return static_cast<RosetteAudioProcessor*>(getAudioProcessor());
};

const RosetteAudioProcessor* RosetteAudioProcessorEditor::getProcessor() const {
    return static_cast<RosetteAudioProcessor*>(getAudioProcessor());
};

void RosetteAudioProcessorEditor::setupDefaultState() {
    updateEditorCache();
}

void RosetteAudioProcessorEditor::queueMidiMessage(juce::MidiMessage msg) {
    auto proc = getProcessor();
    auto buf = proc->getMidiOutMessageBuffer();
    buf->push(std::move(msg));
}

void RosetteAudioProcessorEditor::playbackStateChanged(bool newVal) {
    auto &temp = getTemp();
    temp.shouldUpdateScroll = true;
    
    if (temp.lastMonitoredNote.hasValue()) {
        auto note = *temp.lastMonitoredNote;
        queueMidiMessage(juce::MidiMessage::noteOff(0, note));
        temp.lastMonitoredNote = {};
    }
}

