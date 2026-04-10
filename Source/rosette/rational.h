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

#include <format>
#include <numeric>
#include <string>

namespace roset {
  typedef int percent;
  struct rat {
    int num;
    int den;

    rat(int n, int d) {
      int gcd = std::gcd(d, n);
      this->num = n / gcd;
      this->den = d / gcd;
    }

    rat(int n)
      : num(n)
      , den(1) {}

    rat()
      : num(0)
      , den(1) {}

    bool isZero() const { return this->num == 0; }

    bool isNegative() const { return this->num < 0; }
    bool isPositive() const { return this->num >= 0; }

    float toFloat() const { return (float) num / (float) den; }

    int toTicks(int ppqn) const { return (num * ppqn) / den; }

    int toWhole() const { return num / den; }

    bool divisibleBy(int d) const { return d % this->den == 0; }

    bool operator==(const rat& b) const { return num == b.num && den == b.den; }

    bool operator<(const rat& b) const {
      int lcm = std::lcm(den, b.den);
      int aNum = num * (lcm / den);
      int bNum = b.num * (lcm / b.den);
      return aNum < bNum;
    }

    bool operator>(const rat& b) const {
      int lcm = std::lcm(den, b.den);
      int aNum = num * (lcm / den);
      int bNum = b.num * (lcm / b.den);
      return aNum > bNum;
    }

    bool operator>=(const rat& b) const { return *this == b || *this > b; }

    bool operator<=(const rat& b) const { return *this == b || *this < b; }

    friend rat operator+(const rat& a, const rat& b) {
      int lcm = std::lcm(a.den, b.den);
      int aNum = a.num * (lcm / a.den);
      int bNum = b.num * (lcm / b.den);
      return rat(aNum + bNum, lcm);
    }

    friend rat operator-(const rat& a, const rat& b) {
      int lcm = std::lcm(a.den, b.den);
      int aNum = a.num * (lcm / a.den);
      int bNum = b.num * (lcm / b.den);
      return rat(aNum - bNum, lcm);
    }

    friend rat operator*(const rat& a, const rat& b) { return rat(a.num * b.num, a.den * b.den); }

    friend rat operator/(const rat& a, const rat& b) { return rat(a.num * b.den, a.den * b.num); }

    friend rat operator+=(rat& a, const rat& b) {
      a = a + b;
      return a;
    }

    friend rat operator-=(rat& a, const rat& b) {
      a = a - b;
      return a;
    }

    friend rat operator*=(rat& a, const rat& b) {
      a = a * b;
      return a;
    }

    friend rat operator/=(rat& a, const rat& b) {
      a = a / b;
      return a;
    }

    rat operator-() { return -1 * *this; }

    rat convert(int newDen) const {
      float f = ((float) num / (float) den) * (float) newDen;
      return rat(f, newDen);
    }

    rat quantize(int newDen) const {
      float f = ((float) num / (float) den) * (float) newDen;
      return rat(round(f), newDen);
    }

    rat fractionalPart() const { return rat(num % den, den); }

//    std::string formatInDen(int newDen, bool fromOne = false) const {
//      int m = newDen / den;
//      int n = num * m;
//      return std::format("{}/{}", n + (fromOne ? 1 : 0), newDen);
//    }
  };
} // namespace nu


//template <>
//struct std::formatter<rosette::rat> {
//    constexpr auto parse(std::format_parse_context& ctx) {
//        return ctx.begin();
//    }
//
//    auto format(const rosette::rat& o, std::format_context& ctx) const {
//        if (o.num == 0) {
//            return std::format_to(ctx.out(), "0r");
//        }
//       auto whole = o.num / o.den;
//       if (whole > 0) {
//           if (o.num % o.den == 0) {
//               return std::format_to(ctx.out(),"{}r", whole);
//           }
//           auto part = rosette::rat(o.num % o.den, o.den);
//           return std::format_to(ctx.out(),"{} + {}/{}r", whole, part.num, part.den);
//       } else {
//           return std::format_to(ctx.out(),"{}/{}r", o.num, o.den);
//       }
//    }
//};
