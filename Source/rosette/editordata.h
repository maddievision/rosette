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

namespace rosette {

struct EditorConfigCache {
    std::map<rosette::EffectType, char> effectTypeToCharMap{};
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
};

struct PlaybackStateCache {
    bool wasPlaying{};
    bool isPlaying{};
    rosette::BPM bpm{120};
    rosette::PPQ ppq{};
    rosette::SampleRate sampleRate{};
    std::size_t bufferSize{};
    bool hasCycle{};
    rosette::PPQ cycleStart{};
    rosette::PPQ cycleEnd{};
};

struct DrawingCache {
    std::vector<rosette::span<int>> columnSpans{};
    std::vector<rosette::span<int>> channelSpans{};
};

struct EditorCache {
    EditorTempState temp{};
    PlaybackStateCache playback{};
    EditorConfigCache config{};
    DrawingCache drawing{};
};

}
