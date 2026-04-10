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
#include "PluginProcessor.h"
#include "rosette/rational.h"
#include "rosette/template.h"
#include "rosette/column.h"
#include "rosette/sheet.h"
#include "rosette/savedata.h"
#include "rosette/playback.h"
#include "rosette/editordata.h"

//==============================================================================
/**
*/

class RosetteAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    RosetteAudioProcessorEditor (RosetteAudioProcessor&);
    ~RosetteAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    juce::Slider divisionSlider;
    juce::Label divisionLabel;
    juce::Slider stepSlider;
    juce::Label stepLabel;
    juce::Slider octaveSlider;
    juce::Label octaveLabel;
    juce::Slider instrumentSlider;
    juce::Label instrumentLabel;
    juce::ToggleButton enableStepInputButton;
    juce::ToggleButton enableMidiStepInputButton;
    juce::ToggleButton enableMidiVelocityInputButton;
    juce::ToggleButton enableLiveRecordButton;

    juce::Font* m_font{};

    roset::EditorCache m_cache{};
    
    roset::Sheet &getSheet();
    const roset::Sheet &getSheet() const;
    roset::Sheet &getShadowSheet();
    const roset::Sheet &getShadowSheet() const;
    
    void setupComponents();

    void drawBackground(juce::Graphics& g);
    void drawGridLines(juce::Graphics& g);
    void drawMarkers(juce::Graphics& g);
    void drawStatus(juce::Graphics& g);
    void drawHeader(juce::Graphics& g);
    void drawCorner(juce::Graphics& g);
    void drawRuler(juce::Graphics& g);
    void drawSheet(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g, bool rulerWidth = false);
    void drawCursor(juce::Graphics& g);
    void drawRightMask(juce::Graphics &g);

    void makeUpdates();
    void updateEditorCache();
    
    float getPPQHeight() const;
    roset::bounds<roset::rat> getVisibleTimeArea() const;
    juce::Point<int> getSheetBasePos(bool forGlobal = false, bool applyScroll = true) const;
    void updatePlaybackStateCache();
    double getCurrentTime() const;
    
    roset::rat getStepLength() const;
    void navigateTo(const roset::SheetPoint& position);
    
    void checkUpdateScroll();
    
    juce::Rectangle<int> getCursorRect(bool applyScroll = true);
    int getPlaybackY(bool applyScroll = true);
    
    void insertEvent(const roset::SheetEvent& event, bool advance = true, bool monitorKey = false, juce::Optional<float> withVolume = {});
    void cycleNoteDisplayStyle();
    void clearEvent(bool advance = true);
    void deleteEvent();
    roset::NoteNumber getNoteNumberInCurrentOctave(int baseNote) const;
    void advanceToNextStep();
    
    roset::Column& getCurrentColumn(bool inShadow = false);
    roset::ColAddress getCurrentAddress() const;
    
    
    roset::PluginEditorState &getState();
    const roset::PluginEditorState &getState() const;
    roset::PluginEditorConfig &getConfig();
    const roset::PluginEditorConfig &getConfig() const;
    roset::PluginData &getPluginData();
    const roset::PluginData &getPluginData() const;
    roset::PluginCache &getPluginCache();
    const roset::PluginCache &getPluginCache() const;

    
    roset::EditorCache &getCache();
    const roset::EditorCache &getCache() const;

    roset::PlaybackStateCache &getPlaybackCache();
    const roset::PlaybackStateCache &getPlaybackCache() const;

    roset::DrawingCache &getDrawingCache();
    const roset::DrawingCache &getDrawingCache() const;

    roset::EditorConfigCache &getConfigCache();
    const roset::EditorConfigCache &getConfigCache() const;

    roset::EditorTempState &getTemp();
    const roset::EditorTempState &getTemp() const;
    
    RosetteAudioProcessor* getProcessor();
    const RosetteAudioProcessor* getProcessor() const;
    
    void handleMidiMessage(const RosetteMidiMessage &msg);
    
    void queueMidiMessage(juce::MidiMessage msg);
    
    void playbackStateChanged(bool newVal);


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RosetteAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RosetteAudioProcessorEditor)
    
    void setupDefaultState();
};
