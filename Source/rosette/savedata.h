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
#include "event.h"
#include "sheet.h"

namespace rosette {

struct PresetEffect {
    EffectType type{};
    int param1{};
    int param2{};
};

struct PluginEditorConfig {
    std::map<juce::juce_wchar, int> noteKeyMap{};
    std::map<char, EffectType> charToEffectTypeMap{};
    std::map<char, PresetEffect> quickEffectMap{};
};

struct PluginEditorState {
    SheetRect selection{};
    SheetPoint position{};
    rat division{};
    int step{1};
    int octave{5};
    int instrument{0};
};

struct MouseGrabState {
    bool active{};
    juce::Point<int> positionStart;
    juce::Point<int> scrollPositionStart;
};

// Actual data that gets persisted to plugin state
struct PluginData {
    Sheet sheet{};
    PluginEditorConfig config{};
    PluginEditorState editorState{};
    
    void readState(juce::MemoryBlock& data);
    void saveState(juce::MemoryBlock& data);
    
    void reset();
};

}
