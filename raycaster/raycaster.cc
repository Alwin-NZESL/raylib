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

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "raylib.h"

#include "world_model.h"
#include "world_view.h"

void handle_input( WorldModel &world );
void render_scene( WorldView &world_view, std::vector<uint32_t> &framebuffer, WorldModel &world, const Texture2D &texture );

int main( int argc, char* argv[] )
{
    WorldModel world;
    WorldView view;

    InitWindow( 1024, 800, "Raycaster test" );
    SetTargetFPS( 60 );

    std::vector<uint32_t> framebuffer( 1024 * 800, 0 );
    Image image = {
        .data = framebuffer.data(),
        .width = 1024,
        .height = 800,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    Texture2D texture = LoadTextureFromImage( image );    

    while( !WindowShouldClose() ) {

        handle_input( world );

        world.update( GetFrameTime() * 1000.0F );

        render_scene( view, framebuffer, world, texture );
    }

    UnloadTexture( texture );
    CloseWindow();
}

void handle_input( WorldModel &world )
{
    static std::unordered_map<int, WorldModel::KeyState> key_map = {
        { KEY_UP, WorldModel::KeyState::MoveForward },
        { KEY_DOWN, WorldModel::KeyState::MoveBackward },
        { KEY_LEFT, WorldModel::KeyState::RotateLeft },
        { KEY_RIGHT, WorldModel::KeyState::RotateRight },
        { KEY_X, WorldModel::KeyState::ZoomIn },
        { KEY_Z, WorldModel::KeyState::ZoomOut },
        { KEY_SPACE, WorldModel::KeyState::ToggleMinimap },
        { KEY_T, WorldModel::KeyState::ToggleTextures }
    };

    for( const auto &[key, key_state] : key_map ) {
        if( IsKeyPressed(key) ) world.key_state_action( key_state, true );
        if( IsKeyReleased(key) ) world.key_state_action( key_state, false );
    }
}

void render_scene( WorldView &view, std::vector<uint32_t> &framebuffer, WorldModel &world, const Texture2D &texture )
{
    view.draw_frame( framebuffer.data(), &world, 1024, 800 );

    if( world.do_show_minimap() )
        view.draw_minimap( framebuffer.data(), &world, 1024, 800 );

    UpdateTexture( texture, framebuffer.data() );

    BeginDrawing();

    DrawTexture( texture, 0, 0, WHITE );

    std::string delta_time_text = "Delta time: " + std::to_string( GetFrameTime() * 1000.0F ) + " ms";
    DrawText( delta_time_text.c_str(), 0, 760, 40, RED );
    std::string background_text = "Background: " + std::to_string( view.metrics.background_us / 1000.0F ) + " ms";
    DrawText( background_text.c_str(), 0, 720, 40, GREEN );
    std::string rays_text = "Rays: " + std::to_string( view.metrics.rays_us / 1000.0F ) + " ms";
    DrawText( rays_text.c_str(), 0, 680, 40, BLUE );

    EndDrawing();
}
