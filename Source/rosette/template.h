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

namespace rosette {

template<typename C, typename R>
struct point {
    C col;
    R row;
    
    point<C, R> offsetCol(const C &bCol) {
        return point{.col = col + bCol, .row = row};
    }
    
    point<C, R> offsetRow(const R &bRow) {
        return point{.col = col, .row = row + bRow};
    }

    
    bool operator==(const point<C, R>& b) const { return col == b.col && row == b.row; }
    
    bool operator<(const point<C, R>& b) const {
        return row < b.row || (row == b.row && col < b.col);
    }
    
    bool operator>(const point<C, R>& b) const {
        return row > b.row || (row == b.row && col > b.col);
    }
    
    bool operator>=(const point<C, R>& b) const { return *this == b || *this > b; }
    
    bool operator<=(const point<C, R>& b) const { return *this == b || *this < b; }
    
    friend point<C, R> operator-(const point<C, R>& a, const C& b) {
        return point{.col = a.col - b, .row = a.row};
    }
    
    friend point<C, R> operator+(const point<C, R>& a, const point<C, R>& b) {
        return point{.col = a.col + b.col, .row = a.row + b.row};
    }
    
    friend point<C, R> operator-(const point<C, R>& a, const point<C, R>& b) {
        return point{.col = a.col - b.col, .row = a.row - b.row};
    }
    
    friend point<C, R> operator*(const point<C, R>& a, const point<C, R>& b) {
        return point{.col = a.col * b.col, .row = a.row * b.row};
    }
    
    friend point<C, R> operator/(const point<C, R>& a, const point<C, R>& b) {
        return point{.col = a.col / b.col, .row = a.row / b.row};
    }
    
    friend point<C, R> operator+=(point<C, R>& a, const point<C, R>& b) {
        a = a + b;
        return a;
    }
    
    friend point<C, R> operator-=(point<C, R>& a, const point<C, R>& b) {
        a = a - b;
        return a;
    }
    
    friend point<C, R> operator*=(point<C, R>& a, const point<C, R>& b) {
        a = a * b;
        return a;
    }
    
    friend point<C, R> operator/=(point<C, R>& a, const point<C, R>& b) {
        a = a / b;
        return a;
    }
    
    point<C, R> operator-() { return point<C, R>{-1 * this->col, -1 * this->row}; }
};

template<typename T>
struct bounds {
  T start;
  T end;

  T length() const { return end - start; }
};

typedef int index;

template<typename C, typename R>
struct rect {
    point<C, R> origin;
    point<C, R> extent;
    point<C, R> size() const { return extent - origin; }
    C width() const { return extent.col - origin.col; }
    R height() const { return extent.row - origin.row; }
    rect<C, R> extendCol(C amount) const { return rect<C, R>{.origin = origin, .extent = { .col = extent.col + amount, .row = extent.row }}; }
    rect<C, R> extendRow(R amount) const { return rect<C, R>{.origin = origin, .extent = { .col = extent.col, .row = extent.row + amount }}; }
    rect<C, R> normalize() const {
        point<C, R> newOrigin{ .col = std::min(origin.col, extent.col), .row = std::min(origin.row, extent.row) };
        point<C, R> newExtent{ .col = std::max(origin.col, extent.col), .row = std::max(origin.row, extent.row) };
        return rect<C, R>{ .origin = newOrigin, .extent = newExtent };
    }
};

}
