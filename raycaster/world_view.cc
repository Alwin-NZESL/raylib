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

#include "world_view.h"

#include <cmath>
#include <chrono>

#include "world_model.h"
#include "vec2.h"
#include "adapter.h"
#include "texture_container.h"

namespace {
    TextureContainer textures;

    uint32_t shade_pixel( uint32_t colour, float shading_factor )
    {
        constexpr uint32_t ALPHA_MASK = 0xFF000000;
        constexpr uint32_t RED_MASK   = 0x00FF0000;
        constexpr uint32_t GREEN_MASK = 0x0000FF00;
        constexpr uint32_t BLUE_MASK  = 0x000000FF;

        uint32_t scale = shading_factor * 256;

        return
            ((((colour & RED_MASK  ) * scale) >> 8) & RED_MASK  ) |
            ((((colour & GREEN_MASK) * scale) >> 8) & GREEN_MASK) |
            ((((colour & BLUE_MASK ) * scale) >> 8) & BLUE_MASK ) |
            (colour & ALPHA_MASK);
    }

    void draw_line( uint32_t* buffer, Vec2i dimension, const Vec2& start, const Vec2& end, uint32_t color )
    {
        int x0 = start.x;
        int y0 = start.y;
        int x1 = end.x;
        int y1 = end.y;

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while( true ) {
            if( x0 >= 0 && x0 < dimension.x && y0 >= 0 && y0 < dimension.y )
                buffer[y0 * dimension.x + x0] = color;

            if( x0 == x1 && y0 == y1 )
                break;

            int e2 = err * 2;
            if( e2 > -dy ) { err -= dy; x0 += sx; }
            if( e2 < dx ) { err += dx; y0 += sy; }
        }
    }

    void draw_point( uint32_t* buffer, Vec2i dimension, const Vec2& position, float size, uint32_t color )
    {
        int centerX = position.x;
        int centerY = position.y;
        int radius = size / 2;

        for( int y = -radius; y <= radius; ++y ) {
            for( int x = -radius; x <= radius; ++x ) {
                if( x * x + y * y <= radius * radius ) { // Check if within circle

                    int drawX = centerX + x;
                    int drawY = centerY + y;

                    if( drawX >= 0 && drawX < dimension.x && drawY >= 0 && drawY < dimension.y )
                        buffer[drawY * dimension.x + drawX] = color;
                }
            }
        }
    }

    void draw_rect( uint32_t* buffer, Vec2i dimension, const Vec2 topleft, const Vec2 size, uint32_t color )
    {
        for( int y = topleft.y; y < topleft.y + size.y; ++y ) {
            for( int x = topleft.x; x < topleft.x + size.x; ++x ) {
                if( x >= 0 && x < dimension.x && y >= 0 && y < dimension.y )
                    buffer[(y * dimension.x + x)] = color;
            }
        }
    }

    void draw_pixel( uint32_t* buffer, Vec2i dimension, const Vec2i position, uint32_t color )
    {
        if( position.x >= 0 && position.x < dimension.x && position.y >= 0 && position.y < dimension.y )
            buffer[(position.y * dimension.x + position.x)] = color;
    }

}

void WorldView::setup( size_t width, size_t height )
{
    bounds = Vec2i( width, height );

    unit_size = height / 100;

    framebuffer.resize( width * height );
    std::fill( framebuffer.begin(), framebuffer.end(), 0 );

    alloc_texture_buffer( framebuffer.data(), bounds );
}

void WorldView::teardown()
{
    delete_texture_buffer();
}


void WorldView::render( WorldModel &world )
{
    bool debugging = world.do_show_debugging();

    auto start = std::chrono::high_resolution_clock::now();

    paint_rays( world );

    auto rays_end = std::chrono::high_resolution_clock::now();

    if( world.do_show_minimap() ) {
        paint_minimap( world );
        paint_camera( world );
    }

    auto minimap_end = std::chrono::high_resolution_clock::now();

    draw_texture_buffer( framebuffer.data() );

    if( debugging ) {

        double rays_us = std::chrono::duration_cast<std::chrono::microseconds>( rays_end - start ).count();
        double minimap_us = std::chrono::duration_cast<std::chrono::microseconds>( minimap_end - rays_end ).count();

        draw_debugging_info( bounds, minimap_us, rays_us );
    }
}

void WorldView::paint_rays( WorldModel& world )
{
    for( int x = 0; x < bounds.x; ++x ) {

        float shading_factor = 1.0F;
        float wall_height = 0;
        Vec2 tex_coord{ 0.0, 0.0 };
        int wall_type = -1;

        const auto hit = world.cast_ray( x, bounds.x);
        if( hit ) {
            wall_type = hit->wall_type;
            shading_factor = 1.0F - hit->wall_side * 0.35F;
            wall_height = bounds.y / ( hit->distance_to_wall * world.get_player_zoom() );
            tex_coord = Vec2{ hit->wall_offset, 1.0F / wall_height };
        }
        
        float wall_top    = (bounds.y - wall_height) / 2;
        float wall_bottom = (bounds.y + wall_height) / 2;

        draw_column( x, wall_top, wall_bottom, tex_coord, wall_type, shading_factor);
    }
}

void WorldView::paint_minimap( WorldModel& world )
{
    constexpr std::array<uint32_t,9> colours
    {
        0xFF000000, // Black
        0xFF0000FF, // Blue
        0xFF00FF00, // Green
        0xFF00FFFF, // Cyan
        0xFFFF0000, // Red
        0xFFFF00FF, // Magenta
        0xFFFFFF00, // Yellow
        0xFFFFFFFF, // White
        0xFF808080  // Gray
    };

    size_t xdim = world.get_world_dimension().x;
    size_t ydim = world.get_world_dimension().y;

    for( size_t y = 0; y < ydim; ++y ) {
        for( size_t x = 0; x < xdim; ++x ) {
            auto cell_type = world.get_celltype({x, y});
            if( cell_type < 9 )
                draw_rect( framebuffer.data(), bounds, { x * unit_size, y * unit_size }, {unit_size, unit_size}, colours[cell_type] );
        }
    }
}

void WorldView::paint_camera( WorldModel& world )
{
	constexpr uint32_t red {0xFFFF0000};
	constexpr uint32_t green {0xFF00FF00};
	constexpr uint32_t blue {0xFF0000FF};
	constexpr uint32_t yellow {0xFFFFFF00};

    const Vec2 position = world.get_player_position() * unit_size;
    const float angle = world.get_player_angle();
    const float zoom = world.get_player_zoom();

    const float cos = std::cos( angle );
    const float sin = std::sin( angle );

    const Vec2 left_vec  = Vec2{ (cos + zoom * sin), (sin - zoom * cos) } * unit_size;
    const Vec2 centre_vec = Vec2{ (cos             ), (sin             ) } * unit_size;
    const Vec2 right_vec = Vec2 { (cos - zoom * sin), (sin + zoom * cos) } * unit_size;

    const Vec2 cam_left   = position + left_vec;
    const Vec2 cam_right  = position + right_vec;
	const Vec2 ray_left   = position + left_vec * 2.0F;
    const Vec2 ray_centre = position + centre_vec * 2.0F;
    const Vec2 ray_right  = position + right_vec * 2.0F;

	draw_line( framebuffer.data(), bounds, position, ray_left, blue );
	draw_line( framebuffer.data(), bounds, position, ray_centre, red );
	draw_line( framebuffer.data(), bounds, position, ray_right, blue );

    draw_line( framebuffer.data(), bounds, cam_left, cam_right, green );

	draw_point( framebuffer.data(), bounds, position, 6.0, yellow );
}

void WorldView::draw_column( size_t x, float wall_top, float wall_bottom, Vec2 &tex_coord, int wall_type, float shading_factor )
{
    constexpr uint32_t CEILING_COLOR = 0xFF181818;
    constexpr uint32_t FLOOR_COLOR = 0xFF626262;
    size_t y;
    float dy = tex_coord.y;     // paint_rays stores the delta in the texture coords

    uint32_t * tex_buffer = textures.get_buffer( wall_type );

    tex_coord.y = (wall_top < 0.0F) ? -wall_top * dy : 0.0F;

    for (y = 0; y < wall_top; ++y)
        draw_pixel( framebuffer.data(), bounds, {x,y}, CEILING_COLOR );

    for (; (y < wall_bottom) && (y < bounds.y); ++y, tex_coord.y += dy) {

        uint32_t ray_colour = shade_pixel( textures.get_colour(tex_buffer, tex_coord), shading_factor );

        draw_pixel( framebuffer.data(), bounds, {x,y}, ray_colour );
    }

    for (; y < bounds.y; ++y)
        draw_pixel( framebuffer.data(), bounds, {x,y}, FLOOR_COLOR );
}
