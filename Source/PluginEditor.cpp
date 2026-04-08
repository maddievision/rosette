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
#include "rosette/ui.h"
#include "rosette/presentation.h"
#include "rosette/drawing.h"

//==============================================================================
RosetteAudioProcessorEditor::RosetteAudioProcessorEditor (RosetteAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    setWantsKeyboardFocus(true);
    setupDefaultState();
    juce::FontOptions editorFont {juce::Typeface::createSystemTypefaceFor(BinaryData::FantasqueSansMonoRegular_ttf, BinaryData::FantasqueSansMonoRegular_ttfSize)};
    m_font = new juce::Font(editorFont.withHeight(13));
    
    startTimerHz(60);
}

RosetteAudioProcessorEditor::~RosetteAudioProcessorEditor()
{
    delete(m_font);
}

bool RosetteAudioProcessorEditor::keyPressed(const juce::KeyPress& key) {
    // TODO(ruby): Implement an action system, and key combo -> action map
    if (key == juce::KeyPress::downKey) {
        navigateTo(getState().position.offsetRow(getStepLength()));
        return true;
    } else if (key == juce::KeyPress::upKey) {
        navigateTo(getState().position.offsetRow(-getStepLength()));
        return true;
    } else if (key == juce::KeyPress::leftKey) {
        navigateTo(getState().position.offsetCol(-1));
        return true;
    } else if (key == juce::KeyPress::rightKey) {
        navigateTo(getState().position.offsetCol(1));
        return true;
    }
    
    
    auto c = key.getTextCharacter();
    auto addr = getCurrentAddress();
    
    if (addr.type == rosette::Scope::NOTE) {
        if (getConfig().noteKeyMap.contains(c)) {
            insertEvent(rosette::SheetEvent::note(getNoteNumberInCurrentOctave(getConfig().noteKeyMap[c]), getState().instrument));
            return true;
        } else if (c == '1') {
            insertEvent(rosette::SheetEvent::off());
            return true;
        }
    } else {
        if (c == '!') {
            insertEvent(rosette::SheetEvent::off());
            return true;
        }
    }
    
    if (addr.type == rosette::Scope::MOD) {
        if (getConfig().charToEffectTypeMap.contains(c)) {
            auto effectType = getConfig().charToEffectTypeMap[c];
            insertEvent(rosette::SheetEvent::effect(effectType, 0));
            return true;
        }
        
        if (getConfig().quickEffectMap.contains(c)) {
            const auto &qe = getConfig().quickEffectMap[c];
            insertEvent(rosette::SheetEvent::effect(qe.type, qe.param1, qe.param2));
            return true;
        }
    }
    
    if (key == juce::KeyPress::backspaceKey) {
        clearEvent();
        return true;
    } else if (key == juce::KeyPress::deleteKey) {
        deleteEvent();
        return true;
    }
    
    return false;
}

//==============================================================================
void RosetteAudioProcessorEditor::paint (juce::Graphics& g)
{
    updatePlaybackStateCache();
    g.setFont(*m_font);
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
    auto &pb = getPlaybackCache();
    auto &temp = getTemp();
    if (pb.wasPlaying != pb.isPlaying) {
        temp.shouldUpdateScroll = true;
        pb.wasPlaying = pb.isPlaying;
    }
    checkUpdateScroll();
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
    
    const auto &dw = getDrawingCache();
    g.setColour(rosette::ui_colours::gridlineSecondaryColour);
    drawSpans(dw.columnSpans);
    g.setColour(rosette::ui_colours::gridlinePrimaryColour);
    drawSpans(dw.channelSpans);

    // horizontal lines
    
    auto ppqHeight = getPPQHeight();
    auto timeSpan = getVisibleTimeArea();
    for (rosette::rat t = timeSpan.start; t <= timeSpan.end; t += getState().division) {
        if (t.den == 1) {
            g.setColour(rosette::ui_colours::gridlinePrimaryColour);
        } else {
            g.setColour(rosette::ui_colours::gridlineMinorColour);
        }
        int y = sY + t.toFloat() * ppqHeight;
        g.drawLine(0, y, w, y);
    }
     
    const auto &pb = getPlaybackCache();
    if (pb.hasCycle) {
        g.setColour(rosette::ui_colours::gridlineMarkerColour);
        int y1 = gY + pb.cycleStart * ppqHeight;
        int y2 = gY + pb.cycleEnd * ppqHeight;
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
    
    const auto &pb = getPlaybackCache();
    auto isPlaying = pb.isPlaying;
    auto bpm = pb.bpm;
    auto ppq = pb.ppq;
    auto sr = pb.sampleRate;
    auto bufferSize = pb.bufferSize;
    
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
    auto visibleTime = getVisibleTimeArea();
    
    const auto &dw = getDrawingCache();
    const auto &pb = getPlaybackCache();
    const auto &cfgc = getConfigCache();
    for (const auto &[addr, col] : sheet.columns) {
        const auto &span = dw.columnSpans[colIndex];
        int cX = sX + span.start;
        int cW = span.length();
        for (const auto &[t, ev] : col.events) {
            int eY = sY + t.toFloat() * ppqHeight;
            rosette::rat eLen{};
            if (ev.shadowData.length.isNegative()) {
                eLen = visibleTime.end - t;
            } else {
                eLen = std::max(getState().division, ev.shadowData.length);
            }
            int flags{};
            if (pb.isPlaying && ev.type == rosette::EventType::Note) {
                if (pb.ppq >= t.toFloat() && pb.ppq < (t + eLen).toFloat()) {
                    flags |= rosette::drawing::NoteDrawFlags::Active;
                } else {
                    flags |= rosette::drawing::NoteDrawFlags::Inactive;
                }
            }
            int eH = eLen.toFloat() * ppqHeight;
            int eW = cW - 2; // TODO(ruby): Magic number!
            if (!t.divisibleBy(getState().division.den)) {
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
                    char prefix = '?';
                    if (cfgc.effectTypeToCharMap.contains(ev.effectType)) {
                        prefix = cfgc.effectTypeToCharMap.at(ev.effectType);
                    }
                    juce::String label = juce::String::formatted("%c%02d", prefix, ev.param1);
                    rosette::drawing::drawEffect(g, label, 1.0, rosette::ui_colours::noteVolColour, flags, cX, eY, eW, eH);
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
    int w = rulerWidth ? rosette::ui_metrics::rulerWidth : getLocalBounds().getWidth();
    int pY{};
    const auto &pb = getPlaybackCache();
    if (!pb.isPlaying) {
        pY = getCursorRect().getY();
        g.setColour(rosette::ui_colours::currentRowColour);
    } else {
        pY = getPlaybackY();
        g.setColour(rosette::ui_colours::playTimeColour);
    }
    g.fillRect(0, pY, w, static_cast<int>(rosette::ui_metrics::minRowHeight));
}

void RosetteAudioProcessorEditor::drawCursor(juce::Graphics& g) {
    const auto &pb = getPlaybackCache();
    if (pb.isPlaying) return;
    auto rect = getCursorRect();
    g.setColour(rosette::ui_colours::cursorOutlineColour);
    g.drawRect(rect);
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
    return getPluginData().sheet;
}
const rosette::Sheet &RosetteAudioProcessorEditor::getSheet() const {
    return getPluginData().sheet;
}
rosette::Sheet &RosetteAudioProcessorEditor::getShadowSheet() {
    return getPluginCache().shadowSheet;
}
const rosette::Sheet &RosetteAudioProcessorEditor::getShadowSheet() const {
    return getPluginCache().shadowSheet;
}

void RosetteAudioProcessorEditor::makeUpdates() {
    auto proc = getProcessor();
    proc->makeUpdates();
    updateEditorCache();
}

void RosetteAudioProcessorEditor::updateEditorCache() {
    auto &ceMap = getConfig().charToEffectTypeMap;
    auto &ecMap = getConfigCache().effectTypeToCharMap;
    ecMap.clear();
    for (auto [c, e] : ceMap) {
        ecMap.insert_or_assign(e, c);
    }

    const auto &sheet = getShadowSheet();
    const auto &columns = sheet.getColumns();
    auto &dw = getDrawingCache();
    dw.columnSpans.clear();
    dw.channelSpans.clear();
    
    rosette::ColumnIndex i = 0;
    juce::Optional<rosette::ChannelIndex> lastC{};
    int x = 0;
    int channelX = 0;
    
    auto saveChannelWidth = [&] {
        if (lastC.hasValue()) {
            x += rosette::ui_metrics::columnGap;
            dw.channelSpans.push_back({.start = channelX, .end = x});
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
        dw.columnSpans.push_back({.start = x, .end = x + w});
        x += w;
        ++i;
    }
    saveChannelWidth();
}

float RosetteAudioProcessorEditor::getPPQHeight() const {
    return getState().division.den * rosette::ui_metrics::minRowHeight;
}

rosette::span<rosette::rat> RosetteAudioProcessorEditor::getVisibleTimeArea() const {
    auto bounds = getLocalBounds();
    auto h = bounds.getHeight();
    const auto &temp = getTemp();
    int yStart = temp.scrollPos.y;
    int yEnd = temp.scrollPos.y + h;
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
    const auto &rt = getProcessor()->getRealTimeState();
    auto &pb = getPlaybackCache();
    pb.isPlaying = rt.isPlaying.load();
    pb.bpm = rt.bpm.load();
    pb.ppq = rt.ppq.load();
    pb.sampleRate = rt.sampleRate.load();
    pb.bufferSize = rt.bufferSize.load();
    pb.hasCycle = rt.hasCycle.load();
    pb.cycleStart = rt.cycleStart.load();
    pb.cycleEnd = rt.cycleEnd.load();
}

juce::Point<int> RosetteAudioProcessorEditor::getSheetBasePos(bool forGlobal, bool applyScroll) const {
    int sX = forGlobal ? rosette::ui_metrics::sheetX : rosette::ui_metrics::sheetChanStartX;
    int sY = rosette::ui_metrics::sheetY;
    
    const auto &temp = getTemp();
    
    if (applyScroll) {
        return juce::Point<int>(sX, sY) - temp.scrollPos;
    }
    return juce::Point<int>(sX, sY);
}

double RosetteAudioProcessorEditor::getCurrentTime() const {
    const auto &pb = getPlaybackCache();
    const auto &state = getState();
    if (pb.isPlaying) {
        return pb.ppq;
    } else {
        return state.position.row.toFloat();
    }
}

rosette::rat RosetteAudioProcessorEditor::getStepLength() const {
    const auto &state = getState();
    return state.division * state.step;
}

void RosetteAudioProcessorEditor::navigateTo(const rosette::SheetPoint& position) {
    auto &state = getState();
    auto &pCache = getPluginCache();
    rosette::SheetPoint newPos = position;
    newPos.col = std::clamp(newPos.col, 0, static_cast<int>(pCache.shadowSheet.columnCount()) - 1);
    newPos.row = std::max(rosette::rat{}, newPos.row);
    state.position = newPos;
    auto &temp = getTemp();
    temp.shouldUpdateScroll = true;
}

void RosetteAudioProcessorEditor::checkUpdateScroll() {
    auto &temp = getTemp();
    auto &pb = getPlaybackCache();
    if ((pb.isPlaying || temp.shouldUpdateScroll) && !temp.mouseGrab.active) {
        temp.shouldUpdateScroll = false;
        
        auto bounds = getLocalBounds();
        int w = bounds.getWidth();
        int h = bounds.getHeight();
        bool checkX{};
        int cY{};
        int cX{};
        
        if (pb.isPlaying) {
            cY = getPlaybackY(false);
        } else {
            checkX = true;
            auto rect = getCursorRect(false);
            cX = rect.getX();
            cY = rect.getY();
        }
        
        int scrX = temp.scrollPos.getX();
        int scrY = temp.scrollPos.getY();
        
        if (checkX) {
            if (cX - scrX < rosette::ui_metrics::scrollThresholdX) {
                temp.scrollPos.setX(cX - rosette::ui_metrics::scrollThresholdX);
                if (temp.scrollPos.getX() < 0) temp.scrollPos.setX(0);
            } else if (cX - scrX
                       > w - rosette::ui_metrics::scrollThresholdX - rosette::ui_metrics::minimapWidth) {
                temp.scrollPos.x = cX - w + rosette::ui_metrics::scrollThresholdX + rosette::ui_metrics::minimapWidth;
            }
        }
        
        int topScrollThresholdY = (pb.isPlaying ? (rosette::ui_metrics::scrollThresholdNpY * static_cast<float>(h)) : rosette::ui_metrics::scrollThresholdY);
        int bottomScrollThresholdY = (pb.isPlaying ? (rosette::ui_metrics::scrollThresholdNpY * static_cast<float>(h)) : rosette::ui_metrics::scrollThresholdY);
        
        if (cY - scrY < topScrollThresholdY) {
            temp.scrollPos.setY(cY - topScrollThresholdY);
            if (temp.scrollPos.getY() < 0) temp.scrollPos.setY(0);
        } else if (cY - scrY > h - bottomScrollThresholdY) {
            temp.scrollPos.setY(cY - h + bottomScrollThresholdY);
        }
    }
}

juce::Rectangle<int> RosetteAudioProcessorEditor::getCursorRect(bool applyScroll) {
    auto s = getSheetBasePos(false, applyScroll);
    auto ppqHeight = getPPQHeight();
    int sX = s.getX();
    int sY = s.getY();
    const auto &dw = getDrawingCache();
    const auto &state = getState();
    const auto &span = dw.columnSpans[getState().position.col];
    int cY = sY + state.position.row.toFloat() * ppqHeight;
    int cX = sX + span.start;
    int cW = span.length();
    int cH = rosette::ui_metrics::minRowHeight;
    return juce::Rectangle<int>(cX, cY, cW, cH);
}

int RosetteAudioProcessorEditor::getPlaybackY(bool applyScroll) {
    auto s = getSheetBasePos(false, applyScroll);
    auto ppqHeight = getPPQHeight();
    const auto &pb = getPlaybackCache();
    return s.getY() + pb.ppq * ppqHeight;
}

void RosetteAudioProcessorEditor::insertEvent(const rosette::SheetEvent &event, bool advance) {
    const auto &state = getState();
    auto &sheet = getSheet();
    auto &col = sheet.columnAtIndex(state.position.col);
    const auto &t = state.position.row;
    
    col.events.insert_or_assign(t, event);
    makeUpdates();
    
    if (advance) {
        advanceToNextStep();
    }
}

rosette::NoteNumber RosetteAudioProcessorEditor::getNoteNumberInCurrentOctave(int baseNote) const {
    return baseNote + getState().octave * 12;
}

void RosetteAudioProcessorEditor::clearEvent(bool advance) {
    auto &sheet = getSheet();
    const auto &state = getState();
    auto &col = sheet.columnAtIndex(state.position.col);
    const auto &t = state.position.row;

    if (col.has(t)) {
        col.events.erase(t);
        makeUpdates();
    }
    if (advance) {
        advanceToNextStep();
    }
}

void RosetteAudioProcessorEditor::deleteEvent() {
    // delete everything up until the next step then shift up one step
}

void RosetteAudioProcessorEditor::advanceToNextStep() {
    navigateTo(getState().position.offsetRow(getStepLength()));
}

rosette::Column& RosetteAudioProcessorEditor::getCurrentColumn(bool inShadow) {
    auto &sheet = inShadow ? getShadowSheet() : getSheet();
    return sheet[getState().position.col];
}

rosette::ColAddress RosetteAudioProcessorEditor::getCurrentAddress() const {
    const auto &sheet = getSheet();
    return sheet.getAddress(getState().position.col);
}

rosette::PluginData &RosetteAudioProcessorEditor::getPluginData() {
    auto proc = getProcessor();
    return proc->getPluginData();
}

const rosette::PluginData &RosetteAudioProcessorEditor::getPluginData() const {
    auto proc = getProcessor();
    return proc->getPluginData();
}

rosette::PluginCache &RosetteAudioProcessorEditor::getPluginCache() {
    auto proc = getProcessor();
    return proc->getPluginCache();
}

const rosette::PluginCache &RosetteAudioProcessorEditor::getPluginCache() const {
    auto proc = getProcessor();
    return proc->getPluginCache();
}

rosette::PluginEditorState &RosetteAudioProcessorEditor::getState() {
    return getPluginData().editorState;
}
const rosette::PluginEditorState &RosetteAudioProcessorEditor::getState() const {
    return getPluginData().editorState;
}
rosette::PluginEditorConfig &RosetteAudioProcessorEditor::getConfig() {
    return getPluginData().config;
}
const rosette::PluginEditorConfig &RosetteAudioProcessorEditor::getConfig() const {
    return getPluginData().config;
}

rosette::EditorCache &RosetteAudioProcessorEditor::getCache() {
    return m_cache;
}
const rosette::EditorCache &RosetteAudioProcessorEditor::getCache() const {
    return m_cache;
}

rosette::PlaybackStateCache &RosetteAudioProcessorEditor::getPlaybackCache() {
    return getCache().playback;
}
const rosette::PlaybackStateCache &RosetteAudioProcessorEditor::getPlaybackCache() const {
    return getCache().playback;
}

rosette::DrawingCache &RosetteAudioProcessorEditor::getDrawingCache() {
    return getCache().drawing;
}
const rosette::DrawingCache &RosetteAudioProcessorEditor::getDrawingCache() const {
    return getCache().drawing;
}

rosette::EditorConfigCache &RosetteAudioProcessorEditor::getConfigCache() {
    return getCache().config;
}
const rosette::EditorConfigCache &RosetteAudioProcessorEditor::getConfigCache() const {
    return getCache().config;
}

rosette::EditorTempState &RosetteAudioProcessorEditor::getTemp() {
    return getCache().temp;
}
const rosette::EditorTempState &RosetteAudioProcessorEditor::getTemp() const {
    return getCache().temp;
}

RosetteAudioProcessor* RosetteAudioProcessorEditor::getProcessor() {
    return static_cast<RosetteAudioProcessor*>(getAudioProcessor());
};

const RosetteAudioProcessor* RosetteAudioProcessorEditor::getProcessor() const {
    return static_cast<RosetteAudioProcessor*>(getAudioProcessor());
};

void RosetteAudioProcessorEditor::setupDefaultState() {
    updateEditorCache();
}
