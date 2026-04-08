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
namespace ui_metrics {

constexpr float noteLiftX = -1;
constexpr float noteLiftY = -1;

constexpr float margin = 8;
constexpr float padding = 8;

constexpr float columnGap = 4;
constexpr float noteColWidth = 52;
constexpr float noteColWidthNoInstruments = 24;
constexpr float modColWidth = 24;
constexpr float channelFxColWidth = 30;
constexpr float globalFxColWidth = 30;
constexpr float noteElWidth = 50;
constexpr float noteElWidthNoInstruments = 22;
constexpr float modElWidth = 22;
constexpr float channelFxElWidth = 28;
constexpr float globalFxElWidth = 28;
constexpr float colWidth = noteColWidth + modColWidth + channelFxColWidth;
constexpr float borderRadius = 3;
constexpr float minRowHeight = 14;
constexpr float rulerX = 0;
constexpr float rulerY = 0;
constexpr float rulerWidth = 75;

constexpr float charWidth = 7;
constexpr float channelNameMinDisplayWidth = 80;
constexpr float channelTypeMinDisplayWidth = 40;

constexpr float lineHeight = 14;

constexpr float headerHeight = lineHeight * 6;

constexpr float statusHeight = lineHeight * 1;
constexpr float statusY = 0;
constexpr float statusX = 0;

constexpr float cornerHeaderX = rulerX;
constexpr float cornerWidth = rulerWidth;
constexpr float cornerHeaderY = statusHeight;
constexpr float cornerHeight = headerHeight;

constexpr float headerX = rulerX + rulerWidth;
constexpr float headerY = statusY + statusHeight;

constexpr float sheetX = rulerX + rulerWidth;
constexpr float sheetChanStartX = sheetX + globalFxColWidth;
constexpr float sheetY = headerY + headerHeight;

constexpr float textInsetX = 1;
constexpr float octaveInsetX = 16;
constexpr float secondaryTextInsetX = 28;
constexpr float secondaryTextInsetXNoInst = textInsetX;
constexpr float textInsetY = 1;
constexpr float statusInsetX = 6;

constexpr float volMeterWidth = 3;

// constexpr float textInsetX = 4;
// constexpr float secondaryTextInsetX = 36;
// constexpr float textInsetY = 4;
const int trackControlWidth = 240;

constexpr float scrollThresholdY = 100;
constexpr float scrollThresholdNpY = 0.4f;
constexpr float scrollThresholdX = 100;

constexpr float minimapWidth = 100;
constexpr float marginRight = 1;
constexpr float marginBottom = 1;
constexpr float scrollBarThickness = 8;
constexpr float scrollBarRadius = scrollBarThickness / 2;

// timeline strip
const int timelineHeight = 45;

// playlist
const int playOffsetX = trackControlWidth;
const int playOffsetY = timelineHeight;

// minimap
constexpr float mmCursorHeight = 4.0;
constexpr float mmMarkerHeight = 1.0;
constexpr float mmMarkerLabelWidth = noteElWidth;
constexpr float mmMarkerLabelHeight = minRowHeight;
}


namespace ui_colours {
juce::Colour adjustColourVal(juce::Colour col, float pct, float alpha = 1.0f);

const juce::Colour statusBgColour = juce::Colour(static_cast<juce::uint8>(18), static_cast<juce::uint8>(5), static_cast<juce::uint8>(29), static_cast<juce::uint8>(255));

const juce::Colour bgColour = juce::Colour(static_cast<juce::uint8>(18), static_cast<juce::uint8>(5), static_cast<juce::uint8>(29), static_cast<juce::uint8>(255));
const juce::Colour transparentColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(0));

// event Colours
const juce::Colour metaColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour noteVolColour = juce::Colour(static_cast<juce::uint8>(172), static_cast<juce::uint8>(243), static_cast<juce::uint8>(104), static_cast<juce::uint8>(255));
const juce::Colour notePitchColour = juce::Colour(static_cast<juce::uint8>(250), static_cast<juce::uint8>(200), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255));
const juce::Colour noteMetaColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(103), static_cast<juce::uint8>(205), static_cast<juce::uint8>(255));
const juce::Colour chanVolColour = juce::Colour(static_cast<juce::uint8>(80), static_cast<juce::uint8>(221), static_cast<juce::uint8>(177), static_cast<juce::uint8>(255));
const juce::Colour chanPanColour = juce::Colour(static_cast<juce::uint8>(133), static_cast<juce::uint8>(161), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour globalVolColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(111), static_cast<juce::uint8>(111), static_cast<juce::uint8>(255));
const juce::Colour chanFilterColour = juce::Colour(static_cast<juce::uint8>(182), static_cast<juce::uint8>(133), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));

const juce::Colour eventTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour inactiveNoteColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(255));
 const juce::Colour eventListColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(103), static_cast<juce::uint8>(205), static_cast<juce::uint8>(255));
 const juce::Colour eventListTextColour = juce::Colour(static_cast<juce::uint8>(0xFF), static_cast<juce::uint8>(0xB0), static_cast<juce::uint8>(0xE9), static_cast<juce::uint8>(192));
 const juce::Colour eventListPreColour = juce::Colour(static_cast<juce::uint8>(97), static_cast<juce::uint8>(226), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
//    const juce::Colour eventListPreColour = adjustColourVal(juce::Colour(static_cast<juce::uint8>(97), static_cast<juce::uint8>(226), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255)), 0.7, 1.0);
// const juce::Colour eventListColour = juce::Colour(static_cast<juce::uint8>(151), static_cast<juce::uint8>(255), static_cast<juce::uint8>(85), static_cast<juce::uint8>(255));
//    const juce::Colour eventListColour = adjustColourVal(juce::Colour(static_cast<juce::uint8>(151), static_cast<juce::uint8>(255), static_cast<juce::uint8>(85), static_cast<juce::uint8>(255)), 0.7, 1.0);
//const juce::Colour eventListTextColour = juce::Colour(static_cast<juce::uint8>(0xFF), static_cast<juce::uint8>(0xB0), static_cast<juce::uint8>(0xE9), static_cast<juce::uint8>(192));

const juce::Colour formulaResColour = juce::Colour(static_cast<juce::uint8>(160), static_cast<juce::uint8>(94), static_cast<juce::uint8>(234), static_cast<juce::uint8>(255));
const juce::Colour formulaResTextColour = juce::Colour(static_cast<juce::uint8>(200), static_cast<juce::uint8>(120), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));

const juce::Colour meterLowColour = juce::Colour(static_cast<juce::uint8>(172), static_cast<juce::uint8>(243), static_cast<juce::uint8>(104), static_cast<juce::uint8>(255));
const juce::Colour meterMidColour = juce::Colour(static_cast<juce::uint8>(250), static_cast<juce::uint8>(200), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255));
const juce::Colour meterHighColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(111), static_cast<juce::uint8>(111), static_cast<juce::uint8>(255));
//
//    const std::map<nu::EventCategory, juce::Colour> eventCategoryColourMap
//    = {{nu::EventCategory::TEXT, transparentColour},
//        {nu::EventCategory::OFF, transparentColour},
//        {nu::EventCategory::META, metaColour},
//        {nu::EventCategory::UNKNOWN, metaColour},
//        {nu::EventCategory::SLIDE, metaColour},
//        {nu::EventCategory::REF, noteMetaColour},
//        {nu::EventCategory::MARKER, noteMetaColour},
//        {nu::EventCategory::NOTE_VOLUME, noteVolColour},
//        {nu::EventCategory::NOTE_PITCH, notePitchColour},
//        {nu::EventCategory::NOTE_META, noteMetaColour},
//        {nu::EventCategory::CHANNEL_VOLUME, chanVolColour},
//        {nu::EventCategory::CHANNEL_PAN, chanPanColour},
//        {nu::EventCategory::GLOBAL_VOLUME, globalVolColour},
//        {nu::EventCategory::CHANNEL_FILTER, chanFilterColour}};
//
//    const std::map<nu::EventCategory, juce::Colour> eventCategoryTextColourMap
//    = {{nu::EventCategory::TEXT, metaColour},
//        {nu::EventCategory::OFF, metaColour},
//        {nu::EventCategory::META, metaColour},
//        {nu::EventCategory::UNKNOWN, metaColour},
//        {nu::EventCategory::SLIDE, metaColour},
//        {nu::EventCategory::REF, noteMetaColour},
//        {nu::EventCategory::MARKER, noteMetaColour},
//        {nu::EventCategory::NOTE_VOLUME, noteVolColour},
//        {nu::EventCategory::NOTE_PITCH, notePitchColour},
//        {nu::EventCategory::NOTE_META, noteMetaColour},
//        {nu::EventCategory::CHANNEL_VOLUME, chanVolColour},
//        {nu::EventCategory::CHANNEL_PAN, chanPanColour},
//        {nu::EventCategory::GLOBAL_VOLUME, globalVolColour},
//        {nu::EventCategory::CHANNEL_FILTER, chanFilterColour}};

// channel types
const juce::Colour channelTypeGeneratorColour = chanPanColour;
const juce::Colour channelTypeMIDIColour = noteVolColour;
const juce::Colour channelTypePluginColour = notePitchColour;

// ui
const juce::Colour gridlineMarkerColour = noteMetaColour;
const juce::Colour gridlineMajorPrimaryColour = juce::Colour(static_cast<juce::uint8>(160), static_cast<juce::uint8>(94), static_cast<juce::uint8>(234), static_cast<juce::uint8>(255));
const juce::Colour gridlinePrimaryColour = juce::Colour(static_cast<juce::uint8>(120), static_cast<juce::uint8>(71), static_cast<juce::uint8>(176), static_cast<juce::uint8>(255));
const juce::Colour gridlineSecondaryColour = juce::Colour(static_cast<juce::uint8>(80), static_cast<juce::uint8>(47), static_cast<juce::uint8>(117), static_cast<juce::uint8>(255));
// const juce::Colour gridlineSecondaryColour = juce::Colour(static_cast<juce::uint8>(43), static_cast<juce::uint8>(24), static_cast<juce::uint8>(65), static_cast<juce::uint8>(255));
const juce::Colour gridlineMinorColour = juce::Colour(static_cast<juce::uint8>(32), static_cast<juce::uint8>(9), static_cast<juce::uint8>(51), static_cast<juce::uint8>(255));

const juce::Colour cursorOutlineColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(255), static_cast<juce::uint8>(192), static_cast<juce::uint8>(255));
const juce::Colour selectionOutlineColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(255), static_cast<juce::uint8>(192), static_cast<juce::uint8>(64));
const juce::Colour selectionFillColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(255), static_cast<juce::uint8>(192), static_cast<juce::uint8>(64));

const juce::Colour rulerBgColour = juce::Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255));
const juce::Colour rulerBarTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour rulerBeatTextColour = juce::Colour(static_cast<juce::uint8>(192), static_cast<juce::uint8>(192), static_cast<juce::uint8>(192), static_cast<juce::uint8>(255));
const juce::Colour rulerDivisionTextColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(255));
const juce::Colour rulerCurrentTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(99), static_cast<juce::uint8>(188), static_cast<juce::uint8>(255));

const juce::Colour channelHeaderBgColour = juce::Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255));
const juce::Colour channelHeaderDividerColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(255));
const juce::Colour channelHeaderChannelTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour channelHeaderColumnTypeTextColour = juce::Colour(static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(128), static_cast<juce::uint8>(255));
const juce::Colour channelMuteMaskColour = juce::Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(128));

const juce::Colour cornerVoidBgColour = juce::Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255));
const juce::Colour cornerVoidTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));

const juce::Colour currentRowColour = juce::Colour(static_cast<juce::uint8>(43), static_cast<juce::uint8>(24), static_cast<juce::uint8>(65), static_cast<juce::uint8>(255));
const juce::Colour playTimeColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(192), static_cast<juce::uint8>(32));

const juce::Colour minimapBgColour = gridlineMinorColour;
const juce::Colour minimapMarkerColour = gridlineMarkerColour;

//    constexpr juce::Colour minimapMarkerLabelOutlineColour = adjustColourVal(noteMetaColour, 0.5f);
//    constexpr juce::Colour minimapMarkerLabelColour = adjustColourVal(noteMetaColour, 0.35f);

const juce::Colour minimapMarkerLabelTextColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));
const juce::Colour minimapCursorColour = juce::Colour(static_cast<juce::uint8>(187), static_cast<juce::uint8>(187), static_cast<juce::uint8>(187), static_cast<juce::uint8>(128));
const juce::Colour minimapFocusOutlineColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255));

const juce::Colour scrollBarColour = juce::Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(255), static_cast<juce::uint8>(128));
}

}
