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
    rosette::EditorCache m_cache{};
    
    rosette::Sheet &getSheet();
    const rosette::Sheet &getSheet() const;
    rosette::Sheet &getShadowSheet();
    const rosette::Sheet &getShadowSheet() const;

    void drawBackground(juce::Graphics& g);
    void drawGridLines(juce::Graphics& g);
    void drawStatus(juce::Graphics& g);
    void drawHeader(juce::Graphics& g);
    void drawCorner(juce::Graphics& g);
    void drawRuler(juce::Graphics& g);
    void drawSheet(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g, bool rulerWidth = false);
    void drawCursor(juce::Graphics& g);

    void makeUpdates();
    void updateEditorCache();
    
    float getPPQHeight() const;
    rosette::span<rosette::rat> getVisibleTimeArea() const;
    juce::Point<int> getSheetBasePos(bool forGlobal = false, bool applyScroll = true) const;
    void updatePlaybackStateCache();
    double getCurrentTime() const;
    
    rosette::rat getStepLength() const;
    void navigateTo(const rosette::SheetPoint& position);
    
    void checkUpdateScroll();
    
    juce::Rectangle<int> getCursorRect(bool applyScroll = true);
    int getPlaybackY(bool applyScroll = true);
    
    void insertEvent(const rosette::SheetEvent& event, bool advance = true);
    void clearEvent(bool advance = true);
    void deleteEvent();
    rosette::NoteNumber getNoteNumberInCurrentOctave(int baseNote) const;
    void advanceToNextStep();
    
    rosette::Column& getCurrentColumn(bool inShadow = false);
    rosette::ColAddress getCurrentAddress() const;
    
    
    rosette::PluginEditorState &getState();
    const rosette::PluginEditorState &getState() const;
    rosette::PluginEditorConfig &getConfig();
    const rosette::PluginEditorConfig &getConfig() const;
    rosette::PluginData &getPluginData();
    const rosette::PluginData &getPluginData() const;
    rosette::PluginCache &getPluginCache();
    const rosette::PluginCache &getPluginCache() const;

    
    rosette::EditorCache &getCache();
    const rosette::EditorCache &getCache() const;

    rosette::PlaybackStateCache &getPlaybackCache();
    const rosette::PlaybackStateCache &getPlaybackCache() const;

    rosette::DrawingCache &getDrawingCache();
    const rosette::DrawingCache &getDrawingCache() const;

    rosette::EditorConfigCache &getConfigCache();
    const rosette::EditorConfigCache &getConfigCache() const;

    rosette::EditorTempState &getTemp();
    const rosette::EditorTempState &getTemp() const;
    
    RosetteAudioProcessor* getProcessor();
    const RosetteAudioProcessor* getProcessor() const;


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RosetteAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RosetteAudioProcessorEditor)
    
    void setupDefaultState();
};
