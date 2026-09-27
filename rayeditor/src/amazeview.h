/*
 * amazeui.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "level.h"
#include "messages.h"
#include "textboxwrapper.h"

class AMazeView
{
public:    
    void setup( const Level& state );
    EditorAction render(  const Level& level, int selected_tile );
    void process( const EditorResult& update );

    void transform_coords( UICapture& capture );
    size_t get_side_size() const { return side_size; }

private:
    size_t side_size = 0;

    TextBoxWrapper filename{ 600, 130, 330, 70, "File name:"};
    TextBoxWrapper width{600, 30, 80, 70, "Width"};
    TextBoxWrapper height{700, 30, 80, 70, "Height"};
    TextBoxWrapper message{600, 290, 330, 40, ""};

    void calc_side_size( size_t width, size_t height );
};
