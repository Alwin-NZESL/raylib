/*
 * raycaster.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

 #include <fstream>

#include "raylib.h"

#include "world_model.h"
#include "world_view.h"

#include "adapter.h"

int main( int argc, char* argv[] )
{
    constexpr int screen_width = 1920;
    constexpr int screen_height = 1080;
    const std::string default_level("../rayeditor/levels/old_level.lvl");
    
    WorldModel world;
    WorldView view;

    {
        std::string filename = ( argc > 1 ) ? argv[1] : default_level;
        Level level;

        std::ifstream file( filename );

        if( file >> level )
            world.load_level( level );
    }

    InitWindow( screen_width, screen_height, "A-Maze-Thing" );
    SetTargetFPS( 60 );

    view.setup(screen_width, screen_height);

    while( !WindowShouldClose() ) {

        world.update( get_input(), GetFrameTime() * 1000.0F );

        BeginDrawing();

        view.render( world );

        EndDrawing();
    }

    view.teardown();

    CloseWindow();
}
