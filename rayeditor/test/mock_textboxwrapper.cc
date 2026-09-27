/*
 * mock_textboxwrapper.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

namespace MockTextboxWrapper
{
    std::string width_content = "";
    std::string height_content = "";
    std::string filename_content = "";

    void reset() {
        width_content = "";
        height_content = "";
        filename_content = "";
    }
}

void TextBoxWrapper::render_control() {}
void TextBoxWrapper::render_label() {}


void TextBoxWrapper::set_text( std::string text )
{
    if( label == "File name:" )
        MockTextboxWrapper::filename_content = text;

    if( label == "Width" )
        MockTextboxWrapper::width_content = text;
        
    if( label == "Height" )
        MockTextboxWrapper::height_content = text;
}

std::string TextBoxWrapper::get_text() const
{
    if( label == "File name:" )
        return MockTextboxWrapper::filename_content;

    if( label == "Width" )
        return MockTextboxWrapper::width_content;
        
    if( label == "Height" )
        return MockTextboxWrapper::height_content;

    return "Invalid TextboxWrapper";
}

