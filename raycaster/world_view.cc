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
#include <chrono>

#include "vec2.h"

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

void WorldView::setup(int width, int height)
{
    this->width = width;
    this->height = height;

    framebuffer.resize( width * height );
    std::fill( framebuffer.begin(), framebuffer.end(), 0 );
    Image image = {
        .data = framebuffer.data(),
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    texture = LoadTextureFromImage( image );    
}

void WorldView::render( WorldModel &world )
{
    draw_frame( &world );

    if( world.do_show_minimap() )
        draw_minimap( &world );

    UpdateTexture( texture, framebuffer.data() );

    DrawTexture( texture, 0, 0, WHITE );

    std::string delta_time_text = "Delta time: " + std::to_string( GetFrameTime() * 1000.0F ) + " ms";
    DrawText( delta_time_text.c_str(), 0, 760, 40, RED );
    std::string background_text = "Background: " + std::to_string( metrics.background_us / 1000.0F ) + " ms";
    DrawText( background_text.c_str(), 0, 720, 40, GREEN );
    std::string rays_text = "Rays: " + std::to_string( metrics.rays_us / 1000.0F ) + " ms";
    DrawText( rays_text.c_str(), 0, 680, 40, BLUE );
}

void WorldView::draw_frame( WorldModel* world )
{
    auto start = std::chrono::high_resolution_clock::now();

    paint_background( world );

    auto background_end = std::chrono::high_resolution_clock::now();

    paint_rays( world );

    auto rays_end = std::chrono::high_resolution_clock::now();

    metrics.background_us = std::chrono::duration_cast<std::chrono::microseconds>( background_end - start ).count();
    metrics.rays_us = std::chrono::duration_cast<std::chrono::microseconds>( rays_end - background_end ).count();
}

void WorldView::draw_minimap( WorldModel* world )
{
    paint_minimap( world );
    paint_camera( world );
}

void WorldView::paint_rays( WorldModel* world )
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

        uint32_t* tex_buffer = textures.get_buffer( ray_tex_id );

        float shading_factor = 1.0F - walk_side * 0.35F;

        for( int y = wall_top; y < wall_bottom; ++y ) {

            if( (y<0) || (y>=height) )
                continue;

            uint32_t * ray_pixel = (uint32_t*)(&(framebuffer.data())[y * width + x]);

            Vec2 tex_coord{ (float)wall_offset, (y - wall_top)/(float)wall_height };
            uint32_t ray_colour = textures.get_colour( tex_buffer, tex_coord );

            *ray_pixel = shade_pixel( ray_colour, shading_factor );
        }
    }
}

void WorldView::paint_background( WorldModel* world )
{
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

    int current_floor_tex_id = -1;
    int current_ceil_tex_id = -1;
    uint32_t * floor_tex_buffer = nullptr;
    uint32_t * ceil_tex_buffer = nullptr;

    for( int y = height / 2; y < height; ++y ) {

        // y - eye_z is the distance the ray has moved towards the floor as it has traveled from the eye to the viewing plane
        float row_distance = eye_z * ( eye_to_view / (y - eye_z)); // at row_distance the ray hits the floor
        if( row_distance > 1000.0F ) // don't need to draw the floor if it is too far away
            continue;

        Vec2 hit_point = position + left_ray * row_distance;
        Vec2 step =  (right_ray - left_ray) * (row_distance / width);

        uint32_t * floor_pixel_row = &(framebuffer.data())[y * width];
        uint32_t * ceil_pixel_row = &(framebuffer.data())[(height - y) * width];

        for( int x = 0; x < width; ++x ) {

            auto [floor_tex_id, ceil_tex_id] = world->get_background_ids( hit_point );

            if( floor_tex_id != current_floor_tex_id ) {
                floor_tex_buffer = textures.get_buffer( floor_tex_id );
                current_floor_tex_id = floor_tex_id;
            }

            if( ceil_tex_id != current_ceil_tex_id ) {
                ceil_tex_buffer = textures.get_buffer( ceil_tex_id );
                current_ceil_tex_id = ceil_tex_id;
            }

            Vec2 tex_coord = hit_point - hit_point.floor();

            uint32_t floor_colour = textures.get_colour( floor_tex_buffer, tex_coord );
            uint32_t ceil_colour = textures.get_colour( ceil_tex_buffer, tex_coord );

            floor_pixel_row[x] = shade_pixel( floor_colour, shading_factor );
            ceil_pixel_row[x] = shade_pixel(ceil_colour, shading_factor);

            hit_point = hit_point + step;
        }
    }
}

void WorldView::paint_minimap( WorldModel* world )
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

    size_t xdim = world->get_world_dimension().x;
    size_t ydim = world->get_world_dimension().y;

    for( size_t y = 0; y < ydim; ++y ) {
        for( size_t x = 0; x < xdim; ++x ) {
            uint32_t color = 0xFF000000; // Default to black
            int cell_type = world->get_celltype({x, y});
            if( cell_type < 9 ) {
                for( int py = 0; py < unit_size; ++py ) {
                    for( int px = 0; px < unit_size; ++px ) {
                        (framebuffer.data())[(y * unit_size + py) * width  + (x * unit_size + px)] = colours[cell_type];
                    }
                }
            }
        }
    }
}

void WorldView::paint_camera( WorldModel* world )
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

	draw_line( position, ray_left, blue );
	draw_line( position, ray_centre, red );
	draw_line( position, ray_right, blue );

    draw_line( cam_left, cam_right, green );

	draw_point( position, 6.0, yellow );
}

void WorldView::draw_line( const Vec2& start, const Vec2& end, uint32_t color )
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
        if( x0 >= 0 && x0 < width && y0 >= 0 && y0 < height )
            (framebuffer.data())[y0 * width + x0] = color;

        if( x0 == x1 && y0 == y1 )
            break;

        int e2 = err * 2;
        if( e2 > -dy ) { err -= dy; x0 += sx; }
        if( e2 < dx ) { err += dx; y0 += sy; }
    }
}

void WorldView::draw_point( const Vec2& position, float size, uint32_t color )
{
    int centerX = position.x * unit_size;
    int centerY = position.y * unit_size;
    int radius = size / 2;

    for( int y = -radius; y <= radius; ++y ) {
        for( int x = -radius; x <= radius; ++x ) {
            if( x * x + y * y <= radius * radius ) { // Check if within circle

                int drawX = centerX + x;
                int drawY = centerY + y;

                if( drawX >= 0 && drawX < width && drawY >= 0 && drawY < height )
                    (framebuffer.data())[drawY * width + drawX] = color;
            }
        }
    }
}