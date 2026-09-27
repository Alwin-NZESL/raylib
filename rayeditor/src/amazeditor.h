/*
 * amazeditor.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include <optional>

#include "level.h"
#include "messages.h"

class AMazEditor
{
public:    
    void process( const UICapture& input );
    std::optional<EditorResult> process( const EditorAction& actions );

    int get_selected_tile() const { return selected_tile; }
    const Level& get_level() const { return level; }

private:    
    Level level;
    int selected_tile = 1;
    bool dragging = false;

    EditorResult new_level( std::string width, std::string height );
    EditorResult load_level( std::string filename );
    EditorResult save_level( std::string filename );

    void grab_spawn( int x, int y );
    void drop_spawn();

    void paint_tile( int x, int y );
    void erase_tile( int x, int y );
};
