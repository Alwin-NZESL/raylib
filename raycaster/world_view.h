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

#include "world_model.h"
#include "texture_container.h"

struct Metrics
{
    int64_t background_us;
    int64_t rays_us;
};

class WorldView
{
public:
    void draw_frame( uint32_t* framebuffer, WorldModel* world, int width, int height );
    void draw_minimap( uint32_t* framebuffer, WorldModel* world, int width, int height );

    Metrics metrics;

private:
	size_t unit_size = 15;
    TextureContainer textures;

    void paint_background( uint32_t* framebuffer, WorldModel* world, int width, int height );
    void paint_rays( uint32_t* framebuffer, WorldModel* world, int width, int height );
    void paint_minimap( uint32_t* framebuffer, WorldModel* world, int width, int height );
    void paint_camera( uint32_t* framebuffer, WorldModel* world, int width, int height );

    void draw_line( uint32_t* framebuffer, const Vec2& start, const Vec2& end, uint32_t color );
    void draw_point( uint32_t* framebuffer, const Vec2& position, float size, uint32_t color );
};
