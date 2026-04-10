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

#include "address.h"
#include "rational.h"
#include "column.h"
#include "channel.h"
#include "template.h"
#include <optional>

namespace roset {

using SheetPoint = point<ColumnIndex, rat>;
using SheetRect = rect<ColumnIndex, rat>;

struct Sheet {
    public:
    std::map<ColAddress, Column> columns;
    rat length;
    std::vector<ColAddress> addresses;
    
    void clear() { columns.clear(); }
    Column& getOrInsert(const ColAddress& address) {
        if (columns.contains(address)) {
            return columns.at(address);
        } else {
            Column col{.type = address.type, .channelIndex = address.channelIndex};
            columns.insert({address, col});
            return columns.at(address);
        }
    }
    
    // TODO(ruby): This struct should be managing this rather than its user.
    void cacheAddresses() {
        addresses.clear();
        for (const auto& el : columns) {
            addresses.push_back(el.first);
        }
    }
    
    
    std::size_t channelCount() const {
        std::size_t maxChannel = 0;
        for (const auto &[addr, col] : columns) {
            if (addr.channelIndex > maxChannel) {
                maxChannel = addr.channelIndex;
            }
        }
        return maxChannel + 1;
    }
    
    std::vector<ColAddress>::size_type columnCount() const {
        return columns.size();
    }
    const std::vector<ColAddress> &getColumns() const {
        return addresses;
    }
    std::optional<ColumnIndex> columnIndexForAddress(const ColAddress &address) const {
        for (size_t i = 0; i < addresses.size(); ++i) {
            if (address == addresses.at(i)) return i;
        }
        return {};
    }
    Column& columnAtIndex(ColumnIndex i) {
        const auto addr = addresses.at(i);
        return columns.at(addr);
    }
    ColumnIndex columnIndexForChannel(ChannelIndex c) const {
        for (size_t i = 0; i < addresses.size(); i++) {
            auto address = addresses.at(i);
            if (address.channelIndex == c) return i;
        }
        
        return -1;
    }
    
    std::size_t channelEffectColumnCount(ChannelIndex c) const {
        std::size_t iCount = 0;
        for (const auto& [address, col] : columns) {
            if (address.channelIndex == c && address.type == Scope::CHANNEL) {
                iCount = address.index + 1;
            }
        }
        return iCount;
    }
    
    std::size_t channelEffectColumnCount(const ColAddress& la) const {
        if (la.inChannelScope()) return channelEffectColumnCount(la.channelIndex);
        return 0;
    }
    
    std::size_t noteColumnCount(ChannelIndex c) const {
        std::size_t iCount = 0;
        for (const auto& [address, col] : columns) {
            if (address.channelIndex == c && address.type == Scope::NOTE) {
                iCount = address.noteIndex + 1;
            }
        }
        return iCount;
    }
    
    std::size_t noteColumnCount(const ColAddress& la) const {
        if (la.inChannelScope()) return noteColumnCount(la.channelIndex);
        return 0;
    }
    
    std::size_t modColumnCount(ChannelIndex c, NoteIndex n) const {
        std::size_t iCount = 0;
        for (const auto& [address, col] : columns) {
            if (address.channelIndex == c && address.noteIndex == n && address.type == Scope::MOD) {
                iCount = address.index + 1;
            }
        }
        return iCount;
    }
    
    std::size_t modColumnCount(const ColAddress& la) const {
        if (la.inNoteScope()) return modColumnCount(la.channelIndex, la.noteIndex);
        return 0;
    }
    
    ColAddress getAddress(ColumnIndex col) const { return addresses.at(col); }
    
    inline bool has(const ColAddress& la) const { return columns.contains(la); }
    inline bool has(ColumnIndex col) const { return col < addresses.size(); }
    
    inline bool has(const ColAddress& la, const rat& t) const {
        if (!has(la)) return false;
        auto& c = columns.at(la);
        return c.has(t);
    }
    
    inline bool has(ColumnIndex col, const rat& t) const {
        if (!has(col)) return false;
        auto& c = columns.at(addresses.at(col));
        return c.has(t);
    }
    
    inline Column& operator[](const ColAddress& la) { return columns.at(la); }
    inline Column& operator[](ColumnIndex col) { return columns.at(addresses.at(col)); }
    inline const Column& operator[](const ColAddress& la) const { return columns.at(la); }
    inline const Column& operator[](const size_t col) const {
        return columns.at(addresses.at(col));
    }
    
    inline SheetEvent& at(const ColAddress& la, const rat& t) { return columns.at(la)[t]; }
    inline SheetEvent& at(ColumnIndex col, const rat& t) {
        return columns.at(addresses.at(col))[t];
    }
    inline const SheetEvent& at(const ColAddress& la, const rat& t) const {
        return columns.at(la)[t];
    }
    inline const SheetEvent& at(ColumnIndex col, const rat& t) const {
        return columns.at(addresses.at(col))[t];
    }
};
}


