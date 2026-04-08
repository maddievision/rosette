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
#include "rational.h"
#include "template.h"
#include "column.h"
#include "sheet.h"

//==============================================================================
/**
*/

struct EditorData {
    rosette::Sheet sheet{};
};

struct EditorState {
    rosette::SheetRect selection{};
    rosette::SheetPoint position{};
    rosette::rat division{};
    std::size_t step{};
    juce::Point<int> scrollPos{};
};

struct EditorCache {
    rosette::Sheet shadowSheet{};
    std::vector<rosette::span<int>> columnSpans{};
    std::vector<rosette::span<int>> channelSpans{};
};

struct PlaybackStateCache {
    bool isPlaying{};
    rosette::BPM bpm{120};
    rosette::PPQ ppq{};
    rosette::SampleRate sampleRate{};
    std::size_t bufferSize{};
    bool hasCycle{};
    rosette::PPQ cycleStart{};
    rosette::PPQ cycleEnd{};
};

class RosetteAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    RosetteAudioProcessorEditor (RosetteAudioProcessor&);
    ~RosetteAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    EditorData m_data{};
    EditorState m_state{};
    EditorCache m_cache{};
    PlaybackStateCache m_playback{};
    
    rosette::Sheet &getSheet();
    rosette::Sheet &getShadowSheet();
    
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
    void updateShadow();
    void updatePlaybackData();
    void updateEditorCache();
    
    float getPPQHeight() const;
    rosette::span<rosette::rat> getVisibleTimeArea() const;
    juce::Point<int> getSheetBasePos(bool forGlobal = false) const;
    void updatePlaybackStateCache();
    double getCurrentTime() const;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RosetteAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RosetteAudioProcessorEditor)
};
