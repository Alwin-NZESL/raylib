/*
 * rayeditor.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <optional>

#include "messages.h"

#include "amazeditor.h"
#include "amazeview.h"
#include "raylib_adapter.h"

#include <raylib.h>

int main(int argc, char **argv)
{
    AMazEditor editor;
    AMazeView ui;

    InitWindow(1024, 600, "A-Maze-Thing Leveller");
    SetTargetFPS(60);

    ui.setup( editor.get_level() );

    while( !WindowShouldClose() ) {

        if( auto input = get_input() ) {
            ui.transform_coords( *input );
            editor.process( *input );
        }

        BeginDrawing();

            ClearBackground(DARKGREEN);

            EditorAction action = ui.render( editor.get_level(), editor.get_selected_tile() );

        EndDrawing();

        if( auto update = editor.process( action ) )
            ui.process( *update );
    }

    CloseWindow();

    return 0;
}
