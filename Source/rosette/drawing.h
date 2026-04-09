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

namespace rosette {
namespace drawing {

enum NoteDrawFlags {
    None = 0,
    OffGrid = 1,
    Virtual = 2,
    Inactive = 4,
    Active = 8
};
void drawOff(juce::Graphics& g, int drawFlags, int x, int y, int w, int h);
void drawNote(juce::Graphics& g, int noteNumber, int inst, int displayStyle, float volAmt, int drawFlags, int x, int y, int w, int h);
void drawEffect(juce::Graphics& g, const juce::String &label, float pctAmt, juce::Colour colour, int drawFlags, int x, int y, int w, int h);
}
}
