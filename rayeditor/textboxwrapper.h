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

#include <raygui.h>
#include <algorithm>

class TextBoxWrapper
{
public:    
    TextBoxWrapper( Rectangle b, std::string l ) : bounds(b), label(l) {}

    void render_control()
    {
        GuiLabel({bounds.x, bounds.y, bounds.width, 20}, label.c_str() );

        if( GuiTextBox( {bounds.x, bounds.y + 30, bounds.width, 40}, content.data(), static_cast<int>(content.size()), edit_mode ) )
            edit_mode = !edit_mode;
    }

    std::string get_text() const { return std::string( content.data() ); }

    void set_text( std::string text )
    {
        const auto len = std::min(text.size(), content.size() - 1);
        std::copy_n(text.begin(), len, content.begin());
        content[len] = '\0';        
    }

private:
    std::array<char, 256> content = {};
    Rectangle bounds;
    std::string label;
    bool edit_mode = false;
};
