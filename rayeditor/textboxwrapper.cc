/*
 * textboxwrapper.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "textboxwrapper.h"

#include <algorithm>

#include <raygui.h>

void TextBoxWrapper::render_control()
{
    GuiLabel({left, top, width, 20}, label.c_str() );

    if( GuiTextBox( {left, top + 30, width, 40}, content.data(), static_cast<int>(content.size()), edit_mode ) )
        edit_mode = !edit_mode;
}

void TextBoxWrapper::render_label()
{
    GuiLabel({left, top, width, height}, label.c_str() );
}

void TextBoxWrapper::set_text( std::string text )
{
    const auto len = std::min(text.size(), content.size() - 1);
    std::copy_n(text.begin(), len, content.begin());
    content[len] = '\0';        
}

void TextBoxWrapper::set_label( std::string text )
{
    label = text;
}


