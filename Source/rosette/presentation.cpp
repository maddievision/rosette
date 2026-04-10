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

#include "presentation.h"

namespace roset {
namespace note_display {

// m = 12*log2(f/c5) + 60
float midiNoteFromFreq(uint32_t freq, uint32_t a5_freq) {
    return (12.0f * log2((float) freq / (float) a5_freq)) + 29.0f;
}

juce::Colour HsvToRgb(float h, float s, float v, float a) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r, g, b;
    if (h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (h < 300) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }
    float alpha = a * 255;
    return juce::Colour(static_cast<juce::uint8>((r + m) * 255), static_cast<juce::uint8>((g + m) * 255), static_cast<juce::uint8>((b + m) * 255), static_cast<juce::uint8>(alpha));
}

juce::String getNoteName(int note, int style) {
    int baseNote = note % 12;
    return juce::String::fromUTF8(noteNames[baseNote][style]);
}

juce::Colour getColour(int segments, int note, float sat, float val, float alpha) {
    int hueShift = 120;
    int hue = (((int) round(((float) (note % segments) / (float) segments) * 360.0f)) + hueShift)
    % 360;
    
    return HsvToRgb(hue, sat, val, alpha);
}

}
}
