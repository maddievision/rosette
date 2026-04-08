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

#include "event.h"
#include "rational.h"
#include "enum.h"
#include "channel.h"
#include <map>
#include <cstddef>

namespace rosette {
using ColumnIndex = std::size_t;
struct Column {
public:
  ChannelIndex channelIndex;
  Scope type;
  std::map<rat, SheetEvent> events;

  inline bool has(const rat &t) const { return events.contains(t); }
  inline SheetEvent &operator[](const rat &t) { return events.at(t); }
  inline const SheetEvent &operator[](const rat &t) const { return events.at(t); }
};
}
