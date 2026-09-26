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

#include <amazeditor.h>
#include <amazeui.h>

#include <raylib.h>

std::optional<UICapture> get_input()
{
    if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftPressed, GetMouseX(), GetMouseY() };

    if( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftDown, GetMouseX(), GetMouseY() };

    if( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
        return UICapture {UICapture::Type::RightPressed, GetMouseX(), GetMouseY() };

    if( IsMouseButtonReleased( MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftReleased, GetMouseX(), GetMouseY() };

    return std::nullopt;
}

int main(int argc, char **argv)
{
    AMazEditor editor;
    AMazeUI ui;

    InitWindow(1024, 600, "A-Maze-Thing Leveller");
    SetTargetFPS(60);

    ui.setup( editor.get_level() );

    while( !WindowShouldClose() ) {

        if( auto input = get_input() ) {
            ui.transform_coords( *input );
            editor.process_input( *input );
        }

        BeginDrawing();

            ClearBackground(DARKGREEN);

            EditorAction action = ui.render( editor.get_level(), editor.get_selected_tile() );

        EndDrawing();

        if( auto update = editor.process_action( action ) )
            ui.process( *update );
    }

    CloseWindow();

    return 0;
}
