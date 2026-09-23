/*
 * world_view.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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
#include <vector>

#include "world_model.h"
#include "texture_container.h"

class WorldView
{
public:
    void setup( int width, int height );
    void render( WorldModel &world );
    void teardown() { UnloadTexture( texture ); }

private:
	size_t unit_size = 15;
    TextureContainer textures;
    Texture2D texture;
    std::vector<uint32_t> framebuffer;
    int width;
    int height;

    void paint_rays( WorldModel* world );
    void paint_minimap( WorldModel* world );
    void paint_camera( WorldModel* world );

    void draw_line( const Vec2& start, const Vec2& end, uint32_t color );
    void draw_point( const Vec2& position, float size, uint32_t color );
};
