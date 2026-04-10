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

namespace roset {

struct PluginCache {
    Sheet shadowSheet{};
};

struct PlaybackEvent {
    PPQ ppq{};
    juce::MidiMessage midiEvent{};
};

constexpr int PLAYBACK_CAPACITY = 4096;

struct PlaybackEventList {
    std::size_t size{};
    std::array<PlaybackEvent, PLAYBACK_CAPACITY> data{};
    
    bool push(const PlaybackEvent& ev) {
        if (size >= PLAYBACK_CAPACITY) {
            return false;
        }
        data[size++] = ev;
        return true;
    }
    
    const PlaybackEvent &at(std::size_t index) const {
        return data[index];
    }
};

using ChannelNote = point<NoteNumber, ChannelIndex>;

struct PlaybackData {
    PlaybackEventList events;
};

struct PlaybackState {
    std::atomic<bool> eventsInvalidated{};
    
    bool isPlaying{};
    bool needsResync{};
    std::size_t listPos{};
    juce::SortedSet<ChannelNote> activeNotes{};
    PPQ lastPPQ{};
};

struct NoteColControl {
    juce::Optional<int> lastNote{};
    juce::Optional<int> lastInstrument{};
    juce::Optional<int> lastVelocity{};
};

struct MIDIChannelControl {
    juce::Optional<int> lastProgram;
    float pitchWheelAmount = 0;
    float noteSlideAmount = 0;
    float pitchWheelOffset = 0;
    float bendRange = 2;
    int modWheelAmount = 0;
    int rpnLSB = 0;
    int rpnMSB = 0;
    int dataLSB = 0;
    int dataMSB = 0;
    std::array<uint8_t, 128> cc{};
    int getRPN() const { return rpnMSB << 7 | rpnLSB; }
    int getData() const { return dataMSB << 7 | dataLSB; }
};

struct ChannelMIDIControl {
    int midiChannel{};
    std::vector<NoteColControl> noteCtrl{};
};

struct PlaybackMidiRenderState {
    int ticks{};
    double bpm{};
    int tickRate{};

    std::array<MIDIChannelControl, 16> midiCtrl{};
    std::vector<ChannelMIDIControl> chanCtrl{};
};

}
