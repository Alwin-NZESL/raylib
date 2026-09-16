/*
 * renderer.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <cstdint>

#include <raylib.h>

class Renderer {
public:
    ~Renderer();

    void init( const char * title, uint32_t width, uint32_t height, uint32_t fps );

    void start_frame();
    void end_frame();

    void clear( Color color );
    void text( const char* text, int x, int y, int font_size, Color color );
    void draw_texture( const Texture2D& texture, int x, int y );
};