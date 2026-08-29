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
#include "world_model.h"

#include <cmath>
#include "vec2.h"

uint32_t get_pixel_value( TextureContainer& textures, int texture_id, const Vec2 &tex_coord, float shading_factor )
{
    constexpr uint32_t ALPHA_MASK = 0xFF000000;
    constexpr uint32_t RED_MASK   = 0x00FF0000;
    constexpr uint32_t GREEN_MASK = 0x0000FF00;
    constexpr uint32_t BLUE_MASK  = 0x000000FF;

    uint32_t colour = textures.get_colour( texture_id, tex_coord );
    uint32_t scale = shading_factor * 256;

    return
        ((((colour & RED_MASK  ) * scale) >> 8) & RED_MASK  ) |
        ((((colour & GREEN_MASK) * scale) >> 8) & GREEN_MASK) |
        ((((colour & BLUE_MASK ) * scale) >> 8) & BLUE_MASK ) |
           (colour & ALPHA_MASK);
}

void WorldView::draw_frame( uint32_t* framebuffer, WorldModel* world, int width, int height )
{
    paint_background( framebuffer, world, width, height );
    paint_rays( framebuffer, world, width, height );
}

void WorldView::draw_minimap( uint32_t* framebuffer, WorldModel* world, int width, int height )
{
    paint_minimap( framebuffer, world, width, height );
    paint_camera( framebuffer, world, width, height );
}

void WorldView::paint_rays( uint32_t* framebuffer, WorldModel* world, int width, int height )
{
    float zoom_factor;
    int ray_tex_id;
    int walk_side;
    double wall_offset;

    for( int x = 0; x < width; ++x ) {

		if( ! world->cast_ray( x, width, zoom_factor, ray_tex_id, walk_side, wall_offset ) )
            continue;

        int wall_height = height / zoom_factor;

        int wall_top    = (height - wall_height) / 2;
        int wall_bottom = (height + wall_height) / 2;

        float shading_factor = 1.0F - walk_side * 0.35F;

        for( int y = wall_top; y < wall_bottom; ++y ) {

            if( (y<0) || (y>=height) )
                continue;

            Vec2 tex_coord{ (float)wall_offset, (y - wall_top)/(float)wall_height };

            uint32_t * ray_pixel = (uint32_t*)(&framebuffer[y * width + x]);

            *ray_pixel = get_pixel_value( textures, ray_tex_id, tex_coord, shading_factor );
        }
    }
}

void WorldView::paint_background( uint32_t* framebuffer, WorldModel* world, int width, int height )
{
    // paint background
    const float angle = world->get_player_angle();
    const float zoom = world->get_player_zoom();
    const Vec2 position = world->get_player_position();

    const float cos = std::cos( angle );
    const float sin = std::sin( angle );

    const Vec2 left_ray  { (cos + zoom * sin), (sin - zoom * cos) };
    const Vec2 right_ray { (cos - zoom * sin), (sin + zoom * cos) };

    const float eye_to_view = 1.0F / zoom;        // distance between the eye and the viewing plane
    const float eye_z = height / 2;               // complete distance the ray has to travel downwards in order to hit the floor
    const float shading_factor = 0.75F;

    for( int y = height / 2; y < height; ++y ) {

        // y - eye_z is the distance the ray has moved towards the floor as it has traveled from the eye to the viewing plane
        float row_distance = eye_z * ( eye_to_view / (y - eye_z)); // at row_distance the ray hits the floor
        if( row_distance > 1000.0F ) // don't need to draw the floor if it is too far away
            continue;

        Vec2 hit_point = position + left_ray * row_distance;
        Vec2 step =  (right_ray - left_ray) * (row_distance / width);

        for( int x = 0; x < width; ++x ) {

            int floor_tex_id = world->get_floor_texture_id( hit_point );
            int ceil_tex_id = world->get_ceiling_texture_id( hit_point );

            uint32_t * floor_pixel = &framebuffer[y * width + x];
            uint32_t * ceil_pixel = &framebuffer[(height - y) * width + x];

            Vec2 tex_coord = hit_point - hit_point.floor();

            *floor_pixel = get_pixel_value(textures, floor_tex_id, tex_coord, shading_factor);
            *ceil_pixel = get_pixel_value(textures, ceil_tex_id, tex_coord, shading_factor);

            hit_point = hit_point + step;
        }
    }
}

void WorldView::paint_minimap( uint32_t* framebuffer, WorldModel* world, int width, int height )
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

    for( size_t y = 0; y < world->get_world_dimension().y; ++y ) {
        for( size_t x = 0; x < world->get_world_dimension().x; ++x ) {
            uint32_t color = 0xFF000000; // Default to black
            int cell_type = world->get_celltype({x, y});
            if( cell_type < 9 ) {
                for( int py = 0; py < unit_size; ++py ) {
                    for( int px = 0; px < unit_size; ++px ) {
                        framebuffer[(y * unit_size + py) * 1024 + (x * unit_size + px)] = colours[cell_type];
                    }
                }
            }
        }
    }
}

void WorldView::paint_camera( uint32_t* framebuffer, WorldModel* world, int width, int height )
{
	constexpr uint32_t red {0xFFFF0000};
	constexpr uint32_t green {0xFF00FF00};
	constexpr uint32_t blue {0xFF0000FF};
	constexpr uint32_t yellow {0xFFFFFF00};

    const Vec2 position = world->get_player_position();
    const float angle = world->get_player_angle();
    const float zoom = world->get_player_zoom();

    const float cos = std::cos( angle );
    const float sin = std::sin( angle );

    const Vec2 left_vec  { (cos + zoom * sin), (sin - zoom * cos) };
    const Vec2 centre_vec{ (cos             ), (sin             ) };
    const Vec2 right_vec { (cos - zoom * sin), (sin + zoom * cos) };

    const Vec2 cam_left   = position + left_vec;
    const Vec2 cam_right  = position + right_vec;
	const Vec2 ray_left   = position + left_vec * 2.0F;
    const Vec2 ray_centre = position + centre_vec * 2.0F;
    const Vec2 ray_right  = position + right_vec * 2.0F;

	draw_line( framebuffer, position, ray_left, blue );
	draw_line( framebuffer, position, ray_centre, red );
	draw_line( framebuffer, position, ray_right, blue );

    draw_line( framebuffer, cam_left, cam_right, green );

	draw_point( framebuffer, position, 6.0, yellow );
}

void WorldView::draw_line( uint32_t* framebuffer, const Vec2& start, const Vec2& end, uint32_t color )
{
    int x0 = start.x * unit_size;
    int y0 = start.y * unit_size;
    int x1 = end.x * unit_size;
    int y1 = end.y * unit_size;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while( true ) {
        if( x0 >= 0 && x0 < 1024 && y0 >= 0 && y0 < 800 )
            framebuffer[y0 * 1024 + x0] = color;

        if( x0 == x1 && y0 == y1 )
            break;

        int e2 = err * 2;
        if( e2 > -dy ) { err -= dy; x0 += sx; }
        if( e2 < dx ) { err += dx; y0 += sy; }
    }
}

void WorldView::draw_point( uint32_t* framebuffer, const Vec2& position, float size, uint32_t color )
{
    int centerX = position.x * unit_size;
    int centerY = position.y * unit_size;
    int radius = size / 2;

    for( int y = -radius; y <= radius; ++y ) {
        for( int x = -radius; x <= radius; ++x ) {
            if( x * x + y * y <= radius * radius ) { // Check if within circle

                int drawX = centerX + x;
                int drawY = centerY + y;

                if( drawX >= 0 && drawX < 1024 && drawY >= 0 && drawY < 800 )
                    framebuffer[drawY * 1024 + drawX] = color;
            }
        }
    }
}