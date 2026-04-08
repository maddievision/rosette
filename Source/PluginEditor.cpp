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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ui.h"
#include "presentation.h"
#include "drawing.h"

//==============================================================================
RosetteAudioProcessorEditor::RosetteAudioProcessorEditor (RosetteAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    
    m_state.division = rosette::rat(1, 4);
    
    auto &sheet = getSheet();
    auto &noteCol = sheet.getOrInsert(rosette::ColAddress::note(0, 0));
    auto &modCol = sheet.getOrInsert(rosette::ColAddress::mod(0, 0, 0));
    auto &fxCol = sheet.getOrInsert(rosette::ColAddress::channel(0, 0));
    
    // Dummy sequence for now
    rosette::rat t{};
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(64, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(64, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::off());
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(64, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::off());
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(60, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(64, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::off());
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(67, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::off());
    t += rosette::rat(3, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::note(55, 0));
    t += rosette::rat(1, 4);
    noteCol.events.insert_or_assign(t, rosette::SheetEvent::off());

    makeUpdates();
    startTimerHz(60);
}

RosetteAudioProcessorEditor::~RosetteAudioProcessorEditor()
{
}

//==============================================================================
void RosetteAudioProcessorEditor::paint (juce::Graphics& g)
{
    updatePlaybackStateCache();
    g.setFont(juce::FontOptions("Fantasque Sans Mono", 13.0f, juce::Font::plain));
    drawBackground(g);
    drawPlayhead(g);
    drawGridLines(g);
    drawSheet(g);
    drawCursor(g);
    drawHeader(g);
    drawCorner(g);
    drawRuler(g);
    drawPlayhead(g, true);
    drawStatus(g);
}

void RosetteAudioProcessorEditor::drawBackground(juce::Graphics& g) {
    g.fillAll(rosette::ui_colours::bgColour);
}
void RosetteAudioProcessorEditor::drawGridLines(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    int h = bounds.getHeight();
    
    auto sheetPos = getSheetBasePos();
    auto sheetGlobalPos = getSheetBasePos(true);

    int sX = sheetPos.getX();
    int sY = sheetPos.getY();
    int gX = sheetGlobalPos.getX();
    int gY = sheetGlobalPos.getY();
    // vertical lines

    auto drawSpans = [&](const auto &span) {
        for (const auto &span : span) {
            int x1 = span.start + sX;
            int x2 = span.end + sX;
            g.drawLine(x1, 0, x1, h);
            g.drawLine(x2, 0, x2, h);
        }
    };
    
    g.setColour(rosette::ui_colours::gridlineSecondaryColour);
    drawSpans(m_cache.columnSpans);
    g.setColour(rosette::ui_colours::gridlinePrimaryColour);
    drawSpans(m_cache.channelSpans);

    // horizontal lines
    
    auto ppqHeight = getPPQHeight();
    auto timeSpan = getVisibleTimeArea();
    for (rosette::rat t = timeSpan.start; t <= timeSpan.end; t += m_state.division) {
        if (t.den == 1) {
            g.setColour(rosette::ui_colours::gridlinePrimaryColour);
        } else {
            g.setColour(rosette::ui_colours::gridlineMinorColour);
        }
        int y = sY + t.toFloat() * ppqHeight;
        g.drawLine(0, y, w, y);
    }
        
    if (m_playback.hasCycle) {
        g.setColour(rosette::ui_colours::gridlineMarkerColour);
        int y1 = gY + m_playback.cycleStart * ppqHeight;
        int y2 = gY + m_playback.cycleEnd * ppqHeight;
        g.drawLine(0, y1, w, y1);
        g.drawLine(0, y2, w, y2);
        
        rosette::drawing::drawEffect(g, "LP", 1.0f, rosette::ui_colours::noteMetaColour, rosette::drawing::NoteDrawFlags::None, gX, y1, rosette::ui_metrics::globalFxElWidth, rosette::ui_metrics::minRowHeight);
        rosette::drawing::drawEffect(g, "END", 1.0f, rosette::ui_colours::noteMetaColour, rosette::drawing::NoteDrawFlags::None, gX, y2, rosette::ui_metrics::globalFxElWidth, rosette::ui_metrics::minRowHeight);

    }

}


void RosetteAudioProcessorEditor::drawRuler(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int h = bounds.getHeight();
    int rW = rosette::ui_metrics::rulerWidth;
    int rX = rosette::ui_metrics::rulerX;
    int sY = rosette::ui_metrics::sheetY;
    
    g.setColour(rosette::ui_colours::rulerBgColour);
    g.fillRect(rX, sY, rW, h - sY);
}

void RosetteAudioProcessorEditor::drawStatus(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    g.setColour(rosette::ui_colours::statusBgColour);
    int x = rosette::ui_metrics::statusX;
    int y = rosette::ui_metrics::statusY;
    int h = rosette::ui_metrics::statusHeight;
    g.fillRect(x, y, w - x, h);
    
    auto isPlaying = m_playback.isPlaying;
    auto bpm = m_playback.bpm;
    auto ppq = m_playback.ppq;
    auto sr = m_playback.sampleRate;
    auto bufferSize = m_playback.bufferSize;
    
    g.setColour(rosette::ui_colours::eventTextColour);
    if (isPlaying) {
        auto str = juce::String::formatted("[Playing] Beat: %d, BPM: %d, Sample Rate: %d, Buffer Size: %d", static_cast<int>(ppq) + 1, static_cast<int>(bpm), static_cast<int>(sr), static_cast<int>(bufferSize));
        g.drawText(str, x + rosette::ui_metrics::textInsetX, y, w - x, h, juce::Justification::topLeft);
    } else {
        auto str = juce::String::formatted("[Stopped] Beat: %d, BPM: %d, Sample Rate: %d, Buffer Size: %d", static_cast<int>(ppq) + 1, static_cast<int>(bpm), static_cast<int>(sr), static_cast<int>(bufferSize));
        g.drawText(str, x + rosette::ui_metrics::textInsetX, y, w - x, h, juce::Justification::topLeft);
    }
    
}
void RosetteAudioProcessorEditor::drawHeader(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    g.setColour(rosette::ui_colours::channelHeaderBgColour);
    int x = rosette::ui_metrics::headerX;
    int y = rosette::ui_metrics::headerY;
    int h = rosette::ui_metrics::headerHeight;
    g.fillRect(x, y, w - x, h);
}
void RosetteAudioProcessorEditor::drawCorner(juce::Graphics& g) {
    g.setColour(rosette::ui_colours::channelHeaderBgColour);
    int x = rosette::ui_metrics::cornerHeaderX;
    int y = rosette::ui_metrics::cornerHeaderY;
    int w = rosette::ui_metrics::cornerWidth;
    int h = rosette::ui_metrics::cornerHeight;
    g.fillRect(x, y, w, h);
}
void RosetteAudioProcessorEditor::drawSheet(juce::Graphics& g) {
    const auto &sheet = getShadowSheet();
    auto s = getSheetBasePos();
    int sX = s.getX();
    int sY = s.getY();
    int colIndex = 0;
    auto ppqHeight = getPPQHeight();
    
    for (const auto &[addr, col] : sheet.columns) {
        const auto &span = m_cache.columnSpans[colIndex];
        int cX = sX + span.start;
        int cW = span.length();
        for (const auto &[t, ev] : col.events) {
            int eY = sY + t.toFloat() * ppqHeight;
            rosette::rat eLen = std::max(m_state.division, ev.shadowData.length);
            int flags{};
            if (m_playback.isPlaying && ev.type == rosette::EventType::Note) {
                if (m_playback.ppq >= t.toFloat() && m_playback.ppq < (t + eLen).toFloat()) {
                    flags |= rosette::drawing::NoteDrawFlags::Active;
                } else {
                    flags |= rosette::drawing::NoteDrawFlags::Inactive;
                }
            }
            int eH = eLen.toFloat() * ppqHeight;
            int eW = cW - 2; // TODO(ruby): Magic number!
            if (!t.divisibleBy(m_state.division.den)) {
                flags |= rosette::drawing::NoteDrawFlags::OffGrid;
            }
            
            switch (ev.type) {
                case rosette::EventType::Off:
                    rosette::drawing::drawOff(g, flags, cX, eY, eW, eH);
                    break;
                case rosette::EventType::Note:
                    rosette::drawing::drawNote(g, ev.noteNumber, ev.instrument, ev.shadowData.volAmt, flags, cX, eY, eW, eH);
                    break;
                case rosette::EventType::Effect:
                    rosette::drawing::drawEffect(g, "TODO", 1.0, rosette::ui_colours::metaColour, flags, cX, eY, eW, eH);
                    break;
            }
        }
        ++colIndex;
    }
//    int x = m;
//    int y = m;
//    
//    rosette::drawing::drawNote(g, 64, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawNote(g, 64, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawOff(g, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawNote(g, 64, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawOff(g, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawNote(g, 60, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawNote(g, 64, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawOff(g, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawNote(g, 67, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawOff(g, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    y += h;
//    y += h;
//    rosette::drawing::drawNote(g, 55, 0, 1.0f, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
//    rosette::drawing::drawOff(g, rosette::drawing::NoteDrawFlags::None, x, y, w, h);
//    y += h;
}


void RosetteAudioProcessorEditor::drawPlayhead(juce::Graphics& g, bool rulerWidth) {
    auto s = getSheetBasePos();
    auto ppqHeight = getPPQHeight();
    int w = rulerWidth ? rosette::ui_metrics::rulerWidth : getLocalBounds().getWidth();
    int sY = s.getY();
    if (!m_playback.isPlaying) {
        int pY = sY + ppqHeight * m_state.position.row.toFloat();
        g.setColour(rosette::ui_colours::currentRowColour);
        g.fillRect(0, pY, w, static_cast<int>(rosette::ui_metrics::minRowHeight));
    }
    int pY = sY + ppqHeight * m_playback.ppq;
    g.setColour(rosette::ui_colours::playTimeColour);
    g.fillRect(0, pY, w, static_cast<int>(rosette::ui_metrics::minRowHeight));
}

void RosetteAudioProcessorEditor::drawCursor(juce::Graphics& g) {
    if (m_playback.isPlaying) return;
    auto s = getSheetBasePos();
    auto ppqHeight = getPPQHeight();
    int sX = s.getX();
    int sY = s.getY();
    const auto &span = m_cache.columnSpans[m_state.position.col];
    int cY = m_state.position.row.toFloat() * ppqHeight;
    int cX = span.start;
    int cW = span.length();
    int cH = rosette::ui_metrics::minRowHeight;
    g.setColour(rosette::ui_colours::cursorOutlineColour);
    g.drawRect(sX + cX, sY + cY, cW, cH);
}

void RosetteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void RosetteAudioProcessorEditor::timerCallback() {
    repaint();
}

rosette::Sheet &RosetteAudioProcessorEditor::getSheet() {
    return m_data.sheet;
}
rosette::Sheet &RosetteAudioProcessorEditor::getShadowSheet() {
    return m_cache.shadowSheet;
}

void RosetteAudioProcessorEditor::makeUpdates() {
    updateShadow();
    updatePlaybackData();
    updateEditorCache();
}

void RosetteAudioProcessorEditor::updateShadow() {
    getSheet().cacheAddresses();
    // render to shadow
    m_cache.shadowSheet = m_data.sheet; // temp
    
    getShadowSheet().cacheAddresses();
}

void RosetteAudioProcessorEditor::updatePlaybackData() {
    // render shadow to playback data
}

void RosetteAudioProcessorEditor::updateEditorCache() {
    const auto &sheet = getShadowSheet();
    const auto &columns = sheet.getColumns();
    m_cache.columnSpans.clear();
    m_cache.channelSpans.clear();
    
    rosette::ColumnIndex i = 0;
    juce::Optional<rosette::ChannelIndex> lastC{};
    int x = 0;
    int channelX = 0;
    
    auto saveChannelWidth = [&] {
        if (lastC.hasValue()) {
            x += rosette::ui_metrics::columnGap;
            m_cache.channelSpans.push_back({.start = channelX, .end = x});
        }
    };
    
    for (const auto &col : columns) {
        if (!lastC.hasValue() || *lastC != col.channelIndex) {
            // new channel
            saveChannelWidth();
            channelX = x;
            lastC = col.channelIndex;
        }
        
        int w = 0;
        switch (col.type) {
            case rosette::Scope::NOTE:
                w = rosette::ui_metrics::noteColWidth;
                break;
            case rosette::Scope::MOD:
                w = rosette::ui_metrics::modColWidth;
                break;
            case rosette::Scope::CHANNEL:
                w = rosette::ui_metrics::channelFxColWidth;
                break;
        }
        m_cache.columnSpans.push_back({.start = x, .end = x + w});
        x += w;
        ++i;
    }
    saveChannelWidth();
}

float RosetteAudioProcessorEditor::getPPQHeight() const {
    return m_state.division.den * rosette::ui_metrics::minRowHeight;
}

rosette::span<rosette::rat> RosetteAudioProcessorEditor::getVisibleTimeArea() const {
    auto bounds = getLocalBounds();
    auto h = bounds.getHeight();
    int yStart = m_state.scrollPos.y;
    int yEnd = m_state.scrollPos.y + h;
    float ppqHeight = getPPQHeight();

    rosette::rat tStart = rosette::rat(std::floor(yStart / ppqHeight));
    rosette::rat tEnd = rosette::rat(std::ceil(yEnd / ppqHeight));
    
    if (tStart.isNegative()) {
        tStart = rosette::rat{0};
    }
    
    return rosette::span{
        .start = tStart,
        .end = tEnd
    };
}

void RosetteAudioProcessorEditor::updatePlaybackStateCache() {
    const auto &rt = static_cast<RosetteAudioProcessor*>(getAudioProcessor())->getRealTimeState();
    m_playback.isPlaying = rt.isPlaying.load();
    m_playback.bpm = rt.bpm.load();
    m_playback.ppq = rt.ppq.load();
    m_playback.sampleRate = rt.sampleRate.load();
    m_playback.bufferSize = rt.bufferSize.load();
    m_playback.hasCycle = rt.hasCycle.load();
    m_playback.cycleStart = rt.cycleStart.load();
    m_playback.cycleEnd = rt.cycleEnd.load();
}

juce::Point<int> RosetteAudioProcessorEditor::getSheetBasePos(bool forGlobal) const {
    int sX = forGlobal ? rosette::ui_metrics::sheetX : rosette::ui_metrics::sheetChanStartX;
    int sY = rosette::ui_metrics::sheetY;
    
    return juce::Point<int>(sX, sY) - m_state.scrollPos;
}

double RosetteAudioProcessorEditor::getCurrentTime() const {
    if (m_playback.isPlaying) {
        return m_playback.ppq;
    } else {
        return m_state.position.row.toFloat();
    }
}
