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

#include "drawing.h"
#include "ui.h"
#include "presentation.h"

namespace roset {
namespace drawing {

void drawEffect(juce::Graphics& g, const juce::String &label, float pctAmt, juce::Colour colour, int drawFlags, int x, int y, int w, int h) {
    auto textColour = roset::ui_colours::eventTextColour;
    float opacity = 1.0f;
    
    auto strokeColour = roset::ui_colours::adjustColourVal(colour, 0.5f * opacity);
    auto fillColour = roset::ui_colours::adjustColourVal(colour, 0.35f * opacity);

    if (drawFlags & NoteDrawFlags::OffGrid) {
        fillColour = roset::ui_colours::adjustColourVal(colour, 1.0, 0.5);
        strokeColour = roset::ui_colours::adjustColourVal(colour, 1.0, 0.5f);
        textColour = roset::ui_colours::adjustColourVal(colour, 1.0, 0.5f);
    }
    
    int fillWidth = w * pctAmt;
    g.setColour(fillColour);
    g.fillRoundedRectangle(x, y, fillWidth, h, roset::ui_metrics::borderRadius);
    g.setColour(strokeColour);
    g.drawRoundedRectangle(x, y, w, h, roset::ui_metrics::borderRadius, 1);
    g.setColour(textColour);
    g.drawText(label, x + roset::ui_metrics::textInsetX, y, 100, 100, juce::Justification::topLeft);
}

void drawOff(juce::Graphics& g, int drawFlags, int x, int y, int w, int h) {
    auto textColour = roset::ui_colours::eventTextColour;

    if (drawFlags & NoteDrawFlags::OffGrid) {
        textColour = roset::ui_colours::adjustColourVal(textColour, 1.0, 0.5f);
    }

    g.setColour(textColour);
    g.drawText("===", x + roset::ui_metrics::textInsetX, y, 100, 100, juce::Justification::topLeft);
}

void drawNote(juce::Graphics& g, int noteNumber, int inst, int displayStyle, float volAmt, int drawFlags, int x, int y, int w, int h) {
    auto textColour = roset::ui_colours::metaColour;

    int baseNote = noteNumber % 12;
    int noteColourId = roset::note_display::circleOfFifthsMap[baseNote];
    float sat = 0.5;
    
    auto strokeColour = roset::note_display::getColour(12, noteColourId, sat, 1.0, 1.0);
    auto fillColour = roset::note_display::getColour(12, noteColourId, sat, 0.15 * volAmt, 1.0);
    auto noteName = roset::note_display::getNoteName(baseNote, displayStyle);

    
    if (drawFlags & NoteDrawFlags::Active) {
        x += roset::ui_metrics::noteLiftX;
        y += roset::ui_metrics::noteLiftY;
    } else if (drawFlags & NoteDrawFlags::OffGrid) {
        fillColour = roset::ui_colours::adjustColourVal(fillColour, 1.0, 0.5);
        strokeColour = roset::ui_colours::adjustColourVal(strokeColour, 1.0, 0.5f);
        textColour = roset::ui_colours::adjustColourVal(textColour, 1.0, 0.5f);
    } else if (drawFlags & NoteDrawFlags::Inactive) {
        strokeColour = roset::ui_colours::inactiveNoteColour;
        fillColour = roset::ui_colours::adjustColourVal(strokeColour, 0.15f, 1.0f);
    }
    
    g.setColour(fillColour);
    g.fillRoundedRectangle(x, y, w, h, roset::ui_metrics::borderRadius);
    g.setColour(strokeColour);
    g.drawRoundedRectangle(x, y, w, h, roset::ui_metrics::borderRadius, 1);
    g.setColour(textColour);
    
    g.drawText(noteName, x + roset::ui_metrics::textInsetX, y, 100, 100, juce::Justification::topLeft);
    
    auto octString = juce::String::formatted("%d", noteNumber / 12);
    g.drawText(octString, x + roset::ui_metrics::octaveInsetX, y, 100, 100, juce::Justification::topLeft);
    auto instString = juce::String::formatted("%02d", inst + 1);
    g.drawText(instString, x + roset::ui_metrics::secondaryTextInsetX, y, 100, 100, juce::Justification::topLeft);
}

}
}
