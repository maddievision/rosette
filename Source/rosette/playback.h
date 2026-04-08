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

#include "sheet.h"
#include "address.h"
#include "column.h"
#include "channel.h"
#include "event.h"
#include "common.h"
#include "template.h"

namespace rosette {

struct PluginCache {
    rosette::Sheet shadowSheet{};
};

struct PlaybackTimeInfo {
    PPQ ppq{};
};

struct PlaybackEvent {
    PlaybackTimeInfo time{};
    juce::MidiMessage midiEvent{};
};

using ChannelNote = point<NoteNumber, ChannelIndex>;
using PlaybackEventList = std::vector<rosette::PlaybackEvent>;

struct PlaybackData {
    PlaybackEventList events;
};

struct PlaybackState {
    bool isPlaying{};
    bool needsResync{};
    PlaybackEventList::size_type listPos{};
    juce::SortedSet<ChannelNote> activeNotes{};
    PPQ lastPPQ{};
};

struct PlaybackMidiRenderState {
    
};

}
