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
#include <math.h>
#include <string>

namespace rosette {
namespace note_display {

static const char noteNamesByStyle[2][12][5]
= {{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"},
    {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"}};



//static const char noteNamesByStyle[2][12][5]
//= {{"C", "C♯", "D", "D♯", "E", "F", "F♯", "G", "G♯", "A", "A♯", "B"},
//    {"C", "D♭", "D", "E♭", "E", "F", "G♭", "G", "A♭", "A", "B♭", "B"}};
//
static const int circleOfFifthsMap[] = {0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5};
// static const int circleOfFifthsMap[] = {7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5, 0};

float midiNoteFromFreq(uint32_t freq, uint32_t a5_freq);
juce::Colour HsvToRgb(float h, float s, float v, float a);
juce::Colour getColour(int segments, int note, float sat = 1.0, float val = 1.0, float alpha = 1.0);
juce::String getNoteName(int note, bool includeOctave, bool showFlats = false);

}
}
