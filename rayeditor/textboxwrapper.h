/*
 * textboxwrapper.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <string>
#include <array>

class TextBoxWrapper
{
public:    
    TextBoxWrapper( float left, float top, float width, float height, std::string l )
        : left(left), top(top), width(width), height(height), label(l) {}

    void render_control();
    void render_label();

    void set_label( std::string text );
    void set_text( std::string text );

    std::string get_text() const { return std::string( content.data() ); }

private:
    std::array<char, 256> content = {};
    float left;
    float top;
    float width;
    float height;
    std::string label;
    bool edit_mode = false;
};
