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

#include "address.h"

namespace rosette {
std::string toBB26(int n) {
  static std::vector<char> digits;
  digits.clear();
  int c = 0;
  while (n > 0) {
    int r = n % 26;
    if (r == 0) r = 26;
    char d = 'A' + (r - 1);
    digits.insert(digits.begin(), d);
    n = (n - 1) / 26;
  }
  return std::string(digits.begin(), digits.end());
}
}
