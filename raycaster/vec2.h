/*
 * vec2.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <cmath>

struct Vec2i
{
    size_t x;
    size_t y;

    Vec2i() : x(0), y(0) {}
    Vec2i( size_t x, size_t y ) : x(x), y(y) {}
};

struct Vec2
{
    float x;
    float y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2( float x, float y ) : x(x), y(y) {}
    Vec2( const Vec2i& other ) : x(static_cast<float>(other.x)), y(static_cast<float>(other.y)) {};

    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vec2 normalized() const {
        float len = length();
        return (len > 0) ? Vec2(x / len, y / len) : Vec2(0.0f, 0.0f);
    }

    Vec2 scale(Vec2 other) const {
        return Vec2(x * other.x, y * other.y);
    }

    Vec2i floor() const {
        return {static_cast<size_t>(std::floor(x)), static_cast<size_t>(std::floor(y))};
    }
};

inline Vec2 operator+(const Vec2& a, const Vec2& b) {
    return {a.x + b.x, a.y + b.y};
}

inline Vec2 operator+=(Vec2& a, const Vec2& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline Vec2 operator-=(Vec2& a, const Vec2& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

inline Vec2 operator-(const Vec2& a, const Vec2& b) {
    return {a.x - b.x, a.y - b.y};
}

inline Vec2 operator-(const Vec2& a, const Vec2i& b) {
    return {a.x - static_cast<float>(b.x), a.y - static_cast<float>(b.y)};
}

inline Vec2 operator*(const Vec2& a, float scalar) {
    return {a.x * scalar, a.y * scalar};
}
