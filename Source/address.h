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
#include "enum.h"
#include "rational.h"
#include <string>
#include <format>
#include "channel.h"
#include <cstddef>

namespace rosette {
using NoteIndex = std::size_t;
using Index = std::size_t;
struct ColAddress {
    Scope type{Scope::NOTE};
    ChannelIndex channelIndex{};
    NoteIndex noteIndex{};
    Index index{};
        
    static ColAddress channel(ChannelIndex channelIndex, Index index) {
        return ColAddress{.type = Scope::CHANNEL, .channelIndex = channelIndex, .index = index};
    }
    static ColAddress note(ChannelIndex channelIndex, NoteIndex noteIndex) {
        return ColAddress{.type = Scope::NOTE, .channelIndex = channelIndex, .noteIndex = noteIndex};
    }
    static ColAddress mod(ChannelIndex channelIndex, NoteIndex noteIndex, Index index) {
        return ColAddress{.type = Scope::MOD, .channelIndex = channelIndex, .noteIndex = noteIndex, .index = index};
    }
    
    bool inChannelScope() const {
        return type == Scope::CHANNEL || type == Scope::NOTE || type == Scope::MOD;
    }
    bool inNoteScope() const { return type == Scope::NOTE || type == Scope::MOD; }
    bool inModScope() const { return type == Scope::MOD; }
    
    ColAddress getChannel(Index i) const { return channel(channelIndex, i); }
    ColAddress getNote(NoteIndex i) const { return mod(channelIndex, noteIndex, i); }
    ColAddress getMod(NoteIndex n, Index i) const { return mod(channelIndex, n, i); }
    
    std::size_t sortableID() const {
        switch (type) {
            case Scope::NOTE:
                return ((channelIndex + 1) * 10000) + (noteIndex * 100);
            case Scope::MOD:
                return ((channelIndex + 1) * 10000) + (noteIndex * 100) + (index + 1);
            case Scope::CHANNEL:
                return ((channelIndex + 1) * 10000) + 9900 + (index + 1);
        }
    }
    
    std::size_t sortableIDMidiOrder() const {
        switch (type) {
            case Scope::NOTE:
                return ((channelIndex + 1) * 10100) + (noteIndex * 100);
            case Scope::MOD:
                return ((channelIndex + 1) * 10100) + (noteIndex * 100) + (index + 1);
            case Scope::CHANNEL:
                return ((channelIndex + 1) * 10000) + (index + 1);
        }
    }
    
    bool operator==(const ColAddress& b) const { return sortableID() == b.sortableID(); }
    
    bool operator<(const ColAddress& b) const { return sortableID() < b.sortableID(); }
    
    bool operator>(const ColAddress& b) const { return sortableID() > b.sortableID(); }
};

struct AddrTime {
    ColAddress addr;
    rat t;
    
    bool operator==(const AddrTime& b) const { return t == b.t && addr == b.addr; }
    bool operator<(const AddrTime& b) const { return t < b.t || (t == b.t && addr < b.addr); }
    bool operator>(const AddrTime& b) const { return t > b.t || (t == b.t && addr > b.addr); }
};

std::string toBB26(int n);

} // namespace rosette
//
//template <>
//struct std::formatter<rosette::ColAddress> {
//  constexpr auto parse(std::format_parse_context& ctx) {
//      return ctx.begin();
//  }
//
//  auto format(const rosette::ColAddress& o, std::format_context& ctx) const {
//    switch (o.type) {
//        case rosette::Scope::GLOBAL:
//      return std::format_to(ctx.out(), ":G{}", o.index + 1);
//        case rosette::Scope::CHANNEL:
//      if (o.index > 0) {
//        return std::format_to(ctx.out(), ":C{}F{}", o.channelIndex + 1, o.index + 1);
//      } else {
//        return std::format_to(ctx.out(), ":C{}F", o.channelIndex + 1);
//      }
//        case rosette::Scope::NOTE:
//      if (o.noteIndex > 0) {
//        return std::format_to(ctx.out(), ":C{}N{}", o.channelIndex + 1, o.noteIndex + 1);
//      } else {
//        return std::format_to(ctx.out(), ":C{}", o.channelIndex + 1);
//      }
//        case rosette::Scope::MOD:
//      if (o.noteIndex == 0 && o.index == 0) {
//        return std::format_to(ctx.out(), ":C{}M", o.channelIndex + 1);
//      } else if (o.noteIndex == 0) {
//        return std::format_to(ctx.out(), ":C{}M{}", o.channelIndex + 1, o.index + 1 );
//      } else if (o.noteIndex > 0 && o.index == 0) {
//        return std::format_to(ctx.out(), ":C{}N{}M", o.channelIndex + 1, o.noteIndex + 1);
//      } else {
//        return std::format_to(ctx.out(), ":C{}N{}M{}", o.channelIndex + 1, o.noteIndex + 1, o.index + 1);
//      }
//    }
//    return std::format_to(ctx.out(), "Unknown");
//  }
//};

