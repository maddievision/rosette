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

#include <optional>
#include <variant>
#include <string>
#include "common.h"
#include "rational.h"

namespace rosette {

enum class EventType {
    Off,
    Note,
    Effect,
    Text
};

enum class EffectType {
    Volume,
    Pan
};

struct ShadowData {
    rat length{};
    float volAmt{1.0f};
};

struct SheetEvent {
    EventType type{};
    
    // Note
    NoteNumber noteNumber{};
    int noteDisplayStyle{};
    int instrument{};
    
    // Effect
    EffectType effectType{};
    int param1{};
    int param2{};
    
    // TODO(ruby): Doomed to repeat the same mistakes…
    ShadowData shadowData{};
    
    inline bool isOff() const {
        return type == EventType::Off;
    }
    
    inline bool isEffect() const {
        return type == EventType::Effect;
    }

    inline bool isEffectOfType(EffectType effType) const {
        return isEffect() && effectType == effType;
    }

    inline bool isNote() const {
        return type == EventType::Note;
    }

    static SheetEvent off() {
        return SheetEvent{.type = EventType::Off};
    }
    
    static SheetEvent note(NoteNumber number, int instrument, int noteDisplayStyle = 0) {
        return SheetEvent{.type = EventType::Note, .noteNumber = number, .noteDisplayStyle = noteDisplayStyle, .instrument = instrument};
    }
    
    static SheetEvent effect(EffectType type, int param1, int param2 = 0) {
        return SheetEvent{.type = EventType::Effect, .effectType = type, .param1 = param1, .param2 = param2};
    }
};
}
