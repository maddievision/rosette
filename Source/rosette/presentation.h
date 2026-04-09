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

static const char noteNames[12][4][6] = {
    {"C", "B♯", "D𝄫", "C♮"},
    {"C♯", "D♭", "B𝄪", ""},
    {"D", "C𝄪", "E𝄫", "D♮"},
    {"D♯", "E♭", "F𝄫", ""},
    {"E", "F♭", "D𝄪", "E♮"},
    {"F", "E♯", "G𝄫", "F♮"},
    {"F♯", "G♭", "E𝄪", ""},
    {"G", "F𝄪", "A𝄫", "G♮"},
    {"G♯", "A♭", "", ""},
    {"A", "G𝄪", "B𝄫", "A♮"},
    {"A♯", "B♭", "C𝄫", ""},
    {"B", "C♭", "A𝄪", "B♮"},
};

static const int circleOfFifthsMap[] = {0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5};

float midiNoteFromFreq(uint32_t freq, uint32_t a5_freq);
juce::Colour HsvToRgb(float h, float s, float v, float a);
juce::Colour getColour(int segments, int note, float sat = 1.0, float val = 1.0, float alpha = 1.0);
juce::String getNoteName(int note, int style = 0);

// idk lol
static const char noteLengths[2][65][32] = {
    "1/4",          // 1
    "1/8",          // 2
    "1/8T",         // 3
    "1/16",         // 4
    "1/16 (5)",     // 5
    "1/16T",        // 6
    "1/16 (7)",     // 7
    "1/32",         // 8
    "1/32 (3/3)",   // 9
    "1/32 (5)",     // 10
    "1/32 (11)",    // 11
    "1/32T",        // 12
    "1/32 (13)",    // 13
    "1/32 (7)",     // 14
    "1/32 (5/3)",   // 15
    "1/64"          // 16
    "1/64 (17)",    // 17
    "1/64 (3/3)",   // 18
    "1/64 (19)",    // 19
    "1/64 (5)" ,    // 20
    "1/64 (7/3)",   // 21
    "1/64 (11)",    // 22
    "1/64 (23)",    // 23
    "1/64T",        // 24
    "1/64 (5/5)",   // 25
    "1/64 (13)",    // 26
    "1/64 (3/3/3)", // 27
    "1/64 (7)",     // 28
    "1/64 (29)",    // 29
    "1/64 (5/3)",   // 30
    "1/64 (31)",    // 31
    "1/128",        // 32
    "1/128 (11/3)", // 33
    "1/128 (17)",   // 34
    "1/128 (7/5)",  // 35
    "1/128 (9)",    // 36
    "1/128 (37)",   // 37
    "1/128 (19)",   // 38
    "1/128 (13/3)", // 39
    "1/128 (5)",    // 40
    "1/128 (41)",   // 41
    "1/128 (7/3)",  // 42
    "1/128 (43)",   // 43
    "1/128 (11)",   // 44
    "1/128 (3/3/5)",// 45
    "1/128 (23)",   // 46
    "1/128 (47)",   // 47
    "1/128T",       // 48
    "1/128 (7/7)",  // 49
    "1/128 (5/5)",  // 50
    "1/128 (17/3)", // 51
    "1/128 (13)",   // 52
    "1/128 (53)",   // 53
    "1/128 (3/3/3)",// 54
    "1/128 (11/5)", // 55
    "1/128 (7)",    // 56
    "1/128 (19/3)", // 57
    "1/128 (29)",   // 58
    "1/128 (59)",   // 59
    "1/128 (5/3)",  // 60
    "1/128 (61)",   // 61
    "1/128 (31)",   // 62
    "1/128 (7/3/3)",// 63
    "1/256"         // 64
};

juce::String getNoteLengthName(int division, bool includeOctave, bool showFlats = false);



}
}
