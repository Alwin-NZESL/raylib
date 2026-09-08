/*
 * renderer.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "renderer.h"

#include <raylib.h>

void Renderer::init( const char * title, uint32_t width, uint32_t height, uint32_t fps )
{
    InitWindow(width, height, title);
    SetExitKey(KEY_NULL);
    SetTargetFPS(fps);
}

Renderer::~Renderer() { shutdown(); }
void Renderer::shutdown() { CloseWindow(); }
void Renderer::start_frame() { BeginDrawing(); }
void Renderer::end_frame() { EndDrawing(); }
bool Renderer::is_window_open() { return !WindowShouldClose(); }

void Renderer::clear( Color color )
{
    ClearBackground(color);
}

void Renderer::text( const char* text, int x, int y, int font_size, Color color )
{
    DrawText(text, x, y, font_size, color);
}
