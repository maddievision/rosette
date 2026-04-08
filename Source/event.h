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
    Effect
};

struct ShadowData {
    rat length{};
    float volAmt{1.0f};
};

struct SheetEvent {
    EventType type{};
    
    // Note
    NoteNumber noteNumber{};
    int instrument{};
    
    // Effect
    std::string prefix{};
    int parameter{};
    
    // TODO(ruby): Doomed to repeat the same mistakes…
    ShadowData shadowData{};

    static SheetEvent off() {
        return SheetEvent{.type = EventType::Off};
    }
    
    static SheetEvent note(NoteNumber number, int instrument) {
        return SheetEvent{.type = EventType::Note, .noteNumber = number, .instrument = instrument};
    }
    
    static SheetEvent effect(const std::string& prefix, int parameter) {
        return SheetEvent{.type = EventType::Effect, .prefix = prefix, .parameter = parameter};
    }
};
}
