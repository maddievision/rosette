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
#include "template.h"
#include "common.h"

namespace roset {

struct EditorConfigCache {
    std::map<roset::EffectType, char> effectTypeToCharMap{};
};

enum class EffectEntryStateType {
    None,
    Dec,
    Hex
};

struct EffectEntryState {
    EffectEntryStateType type{};
    int digitsRemaining{};
};

struct EditorTempState {
    EffectEntryState effectEntry{};
    MouseGrabState mouseGrab{};
    bool shouldUpdateScroll{};
    juce::Point<int> scrollPos{};
    bool capturingLiveMidi{};
    juce::Optional<int> lastMonitoredNote{};
};

struct PlaybackStateCache {
    bool wasPlaying{};
    bool isPlaying{};
    BPM bpm{120};
    PPQ ppq{};
    SampleRate sampleRate{};
    std::size_t bufferSize{};
    bool hasCycle{};
    PPQ cycleStart{};
    PPQ cycleEnd{};
};

struct DrawingCache {
    std::vector<bounds<int>> columnBounds{};
    std::vector<bounds<int>> channelBounds{};
};

struct EditorCache {
    EditorTempState temp{};
    PlaybackStateCache playback{};
    EditorConfigCache config{};
    DrawingCache drawing{};
};

}
