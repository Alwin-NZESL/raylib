/*
 * raylib_adapter.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <unordered_map>
#include <string>

#include "adapter.h"
#include "key_messages.h"

#include "vec2.h"

#include <raylib.h>

namespace {
    Texture2D texture;
}

uint16_t get_input( )
{
    uint16_t keystate = 0;

    static std::unordered_map<int, KeyState> key_map = {
        { KEY_UP, KeyState::MoveForward },
        { KEY_DOWN, KeyState::MoveBackward },
        { KEY_LEFT, KeyState::RotateLeft },
        { KEY_RIGHT, KeyState::RotateRight },
        { KEY_X, KeyState::ZoomIn },
        { KEY_Z, KeyState::ZoomOut },
    };

    static std::unordered_map<int, KeyState> key_map2 = {
        { KEY_SPACE, KeyState::ToggleMinimap },
        { KEY_T, KeyState::ToggleTextures },
        { KEY_D, KeyState::ToggleDebugging }
    };

    for( const auto &[key, key_state] : key_map ) {
        if( IsKeyDown(key) ) keystate |= key_state;
    }

    for( const auto &[key, key_state] : key_map2 ) {
        if( IsKeyPressed(key) ) keystate |= key_state;
        if( IsKeyReleased(key) ) keystate &= ~key_state;
    }

    return keystate;
}


void alloc_texture_buffer( uint32_t* buffer, Vec2i dimension )
{
    Image image = {
        .data = buffer,
        .width = (int)dimension.x,
        .height = (int)dimension.y,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    texture = LoadTextureFromImage( image );    

}

void draw_texture_buffer( uint32_t* buffer )
{
    UpdateTexture( texture, buffer );
    DrawTexture( texture, 0, 0, WHITE );
}

void delete_texture_buffer()
{
    UnloadTexture( texture );
}

void draw_debugging_info( Vec2i dimension, double minimap_us, double rays_us )
{
    std::string minimap_text = "Minimap: " + std::to_string( minimap_us / 1000.0F ) + " ms";
    std::string rays_text = "Rays: " + std::to_string( rays_us / 1000.0F ) + " ms";
    std::string delta_time_text = "Delta time: " + std::to_string( GetFrameTime() * 1000.0F ) + " ms";

    DrawText( rays_text.c_str(), 0, dimension.y - 120, 40, BLUE );
    DrawText( minimap_text.c_str(), 0, dimension.y - 80, 40, BLUE );
    DrawText( delta_time_text.c_str(), 0, dimension.y - 40, 40, RED );
}

