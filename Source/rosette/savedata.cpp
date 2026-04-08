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

#include "savedata.h"

namespace rosette {

constexpr int MIN_ROSEVER = 0;
constexpr int THIS_ROSEVER = 0;
constexpr int MAX_ROSEVER = 0;

void PluginData::readState(juce::MemoryBlock& data) {
    reset();
    
    // TODO(ruby) Error results
    
    if (data.getSize() < 4) {
        return;
    }

    juce::MemoryInputStream stream(data, false);
    char header[4];
    stream.read(header, 4);
    if (std::strncmp(header, "RSET", 4) != 0) {
        return;
    }
    auto fileSize = stream.readInt();
    
    stream.read(header, 4);
    if (std::strncmp(header, "VERS", 4) != 0) {
        return;
    }

    auto verSize = stream.readInt();
    if (verSize != 4) {
        return;
    }
    
    int roseVer = stream.readInt();
    if (roseVer < MIN_ROSEVER || roseVer > MAX_ROSEVER) {
        return;
    }
    
    stream.read(header, 4);
    if (std::strncmp(header, "SHEE", 4) == 0) {
        sheet.clear();

        auto blockSize = stream.readInt();
        auto columnCount = stream.readInt();
        
        for (int colIndex = 0; colIndex < columnCount; ++colIndex) {
            Scope type = static_cast<Scope>(stream.readInt());
            auto channelIndex = stream.readInt();
            auto noteIndex = stream.readInt();
            auto index = stream.readInt();
            ColAddress addr{.type = type, .channelIndex = channelIndex, .noteIndex = noteIndex, .index = index};
            auto &col = sheet.getOrInsert(addr);
            
            auto eventCount = stream.readInt();
            
            for (int evIndex = 0; evIndex < eventCount; ++evIndex) {
                auto num = stream.readInt();
                auto den = stream.readInt();
                rat t(num, den);
                                
                EventType type = static_cast<EventType>(stream.readInt());
                juce::Optional<SheetEvent> ev{};
                auto eventSize = stream.readInt();
                switch (type) {
                    case EventType::Off: {
                        if (eventSize != 0) {
                            return;
                        }
                        ev = SheetEvent::off();
                        break;
                    }
                    case EventType::Note: {
                        if (eventSize != 8) {
                            return;
                        }
                        auto noteNumber = stream.readInt();
                        auto instrument = stream.readInt();
                        ev = SheetEvent::note(noteNumber, instrument);
                        break;
                    }
                    case EventType::Effect: {
                        if (eventSize != 16) {
                            return;
                        }
                        EffectType effType = static_cast<EffectType>(stream.readInt());
                        auto effSize = stream.readInt();
                        if (effSize != 8) {
                            return;
                        }
                        auto param1 = stream.readInt();
                        auto param2 = stream.readInt();
                        
                        ev = SheetEvent::effect(effType, param1, param2);
                        break;
                    }
                    default: {
                        return;
                        //                        auto pos = stream.getPosition() + eventSize;
                        //                        stream.setPosition(pos);
                    }
                }
                if (ev.hasValue()) {
                    col.events.insert_or_assign(t, *ev);
                }
            }
        }
    }
}
void PluginData::saveState(juce::MemoryBlock& data) {
    juce::MemoryOutputStream stream(data, false);
    // RSET header
    stream.write("RSET", 4);
    auto offsetFileSize = stream.getPosition();
    stream.writeInt(0); // file size placeholder

    // VERS block
    stream.write("VERS", 4);
    stream.writeInt(4); // block size
    stream.writeInt(THIS_ROSEVER);
    
    stream.write("SHEE", 4);
    auto offsetSheetSize = stream.getPosition();
    stream.writeInt(0);
    stream.writeInt(sheet.columnCount());
    for (const auto &[addr, col] : sheet.columns) {
        stream.writeInt(static_cast<int>(addr.type));
        stream.writeInt(addr.channelIndex);
        stream.writeInt(addr.noteIndex);
        stream.writeInt(addr.index);
        stream.writeInt(col.events.size());
        
        for (const auto &[t, ev] : col.events) {
            stream.writeInt(t.num);
            stream.writeInt(t.den);
            stream.writeInt(static_cast<int>(ev.type));
            switch (ev.type) {
                case rosette::EventType::Off:
                    stream.writeInt(0); // event size
                    break;
                case rosette::EventType::Note:
                    stream.writeInt(8); // event size
                    stream.writeInt(ev.noteNumber);
                    stream.writeInt(ev.instrument);
                    break;
                case rosette::EventType::Effect:
                    stream.writeInt(16); // event size
                    stream.writeInt(static_cast<int>(ev.effectType));
                    stream.writeInt(8); // effect size
                    stream.writeInt(ev.param1);
                    stream.writeInt(ev.param2);
                    break;
            }
        }
    }
    
    auto pos = stream.getPosition();
    stream.setPosition(offsetSheetSize);
    stream.writeInt(pos - offsetSheetSize - 4);
    
    stream.setPosition(offsetFileSize);
    stream.writeInt(pos - offsetFileSize - 4);
    
    stream.setPosition(pos);
}
void PluginData::reset() {
    sheet = {};
    config = {};
    editorState = {};

    editorState.division = rosette::rat(1, 4);
    
    sheet.getOrInsert(rosette::ColAddress::note(0, 0));
    sheet.getOrInsert(rosette::ColAddress::mod(0, 0, 0));
    sheet.getOrInsert(rosette::ColAddress::note(0, 1));
    sheet.getOrInsert(rosette::ColAddress::mod(0, 1, 0));
    sheet.getOrInsert(rosette::ColAddress::note(0, 2));
    sheet.getOrInsert(rosette::ColAddress::mod(0, 2, 0));
    sheet.getOrInsert(rosette::ColAddress::channel(0, 0));
    
    auto &km = config.noteKeyMap;
    km.insert_or_assign('z', 0);  // C 1
    km.insert_or_assign('s', 1);  // C#1
    km.insert_or_assign('x', 2);  // D 1
    km.insert_or_assign('d', 3);  // D#1
    km.insert_or_assign('c', 4);  // E 1
    km.insert_or_assign('v', 5);  // F 1
    km.insert_or_assign('g', 6);  // F#1
    km.insert_or_assign('b', 7);  // G 1
    km.insert_or_assign('h', 8);  // G#1
    km.insert_or_assign('n', 9);  // A 1
    km.insert_or_assign('j', 10); // A#1
    km.insert_or_assign('m', 11); // B 1
    km.insert_or_assign(',', 12); // C 2
    km.insert_or_assign('l', 13); // C#2
    km.insert_or_assign('.', 14); // D 2
    
    km.insert_or_assign('q', 12); // C 2
    km.insert_or_assign('2', 13); // C#2
    km.insert_or_assign('w', 14); // D 2
    km.insert_or_assign('3', 15); // D#2
    km.insert_or_assign('e', 16); // E 2
    km.insert_or_assign('r', 17); // F 2
    km.insert_or_assign('5', 18); // F#2
    km.insert_or_assign('t', 19); // G 2
    km.insert_or_assign('6', 20); // G#2
    km.insert_or_assign('y', 21); // A 2
    km.insert_or_assign('7', 22); // A#2
    km.insert_or_assign('u', 23); // B 2
    km.insert_or_assign('i', 24); // C 3
    km.insert_or_assign('9', 25); // C#3
    km.insert_or_assign('o', 26); // D 3
    km.insert_or_assign('0', 27); // D#3
    km.insert_or_assign('p', 28); // E 3
    
    auto &ceMap = config.charToEffectTypeMap;
    
    ceMap.insert_or_assign('v', rosette::EffectType::Volume);
    
    auto &qfMap = config.quickEffectMap;
    qfMap.insert_or_assign('0', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 0});
    qfMap.insert_or_assign('1', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 11});
    qfMap.insert_or_assign('2', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 22});
    qfMap.insert_or_assign('3', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 33});
    qfMap.insert_or_assign('4', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 44});
    qfMap.insert_or_assign('5', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 55});
    qfMap.insert_or_assign('6', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 66});
    qfMap.insert_or_assign('7', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 77});
    qfMap.insert_or_assign('8', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 88});
    qfMap.insert_or_assign('9', rosette::PresetEffect{.type = rosette::EffectType::Volume, .param1 = 99});
}

}
