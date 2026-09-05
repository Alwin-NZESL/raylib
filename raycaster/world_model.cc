/*
 * world_model.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "world_model.h"

void WorldModel::key_state_action( WorldModel::KeyState key_state, bool is_pressed )
{
	if( is_pressed )
		this->key_state |= key_state;
	else
		this->key_state &= ~key_state;
}

std::pair<int,int> WorldModel::get_background_ids( Vec2 hitpoint )
{
    Vec2i cell = hitpoint.floor();
    // int floor_tex_id = ((int(hitpoint.x + hitpoint.y)) & 1) ? 4 : 3; // diagonal floor
    // int floor_tex_id = ((cell.x + cell.y) & 1) ? 4 : 3; // checkered floor
    // int floor_tex_id = ((int(cell.x )) & 1) ? 4 : 3; // vertical stripes floor
    int floor_tex_id = 3;
    int ceil_tex_id = 5;

    if( show_generated_textures ) {
        floor_tex_id += 8;
        ceil_tex_id += 8;
    }
    
    return std::make_pair( floor_tex_id, ceil_tex_id );
}

int WorldModel::get_wall_texture_id( Vec2 hitpoint )
{
    Vec2i cell = hitpoint.floor();
    int wall_text_id = level[ cell.x + cell.y * world_dimension.x ] - '1';
    
    if( show_generated_textures )
        wall_text_id += 8;

    return wall_text_id;
}

bool WorldModel::update( float elapsed_time )
{
    constexpr float position_speed = 0.005;
    constexpr float rotation_speed = 0.314159265358979323846F / 180.0F;
    constexpr float zoom_speed = 0.001;

    const Vec2 position_adjustment {
        position_speed * elapsed_time * std::cos( player_angle ),
        position_speed * elapsed_time * std::sin( player_angle )
    };

	if( (( key_state & KeyState::MoveForward ) != 0 ) && !is_wall( player_position + position_adjustment ) )
		player_position += position_adjustment;
	if( (( key_state & KeyState::MoveBackward ) != 0 ) && !is_wall( player_position - position_adjustment ) )
		player_position -= position_adjustment;

	if( key_state & KeyState::RotateLeft )
		player_angle -= rotation_speed * elapsed_time;
	if( key_state & KeyState::RotateRight )
		player_angle += rotation_speed * elapsed_time;

    if( key_state & KeyState::ZoomIn )
		player_zoom -= zoom_speed * elapsed_time;
	if( key_state & KeyState::ZoomOut )
		player_zoom += zoom_speed * elapsed_time;

	if( key_state & KeyState::ToggleMinimap ) {
		key_state &= ~KeyState::ToggleMinimap;
		show_minimap = !show_minimap;
	}

	if( key_state & KeyState::ToggleTextures ) {
		key_state &= ~KeyState::ToggleTextures;
		show_generated_textures = !show_generated_textures;
	}

    return true;
}

bool WorldModel::is_wall( Vec2 position )
{
    Vec2i cell = position.floor();  

	if( cell.x < 0 || cell.x > world_dimension.x ||
        cell.y < 0 || cell.y > world_dimension.y
    )
		return false;

	return level[cell.x + cell.y * world_dimension.x] != '0';
}

bool WorldModel::cast_ray( int step, int width, float& zoom_factor, int & cell_type, int& walk_side, double& wall_offset )
{
	constexpr size_t x_dim = 0;
	constexpr size_t y_dim = 1;

	float rot_angle = angle_start() + step * angle_step( width );

	const Vec2 ray_dir { std::cos( rot_angle ), std::sin( rot_angle ) };

	Vec2 cell_to_test = player_position.floor();
    const Vec2 cell_adjust { ( ray_dir.x < 0 ) ? -1.0F : 1.0F, ( ray_dir.y < 0 ) ? -1.0F : 1.0F };
    const std::array<float, 2> step_size = calc_step_size( ray_dir );
    std::array<float, 2> ray_length = calc_initial_ray_lengths( player_position, ray_dir, step_size );

    bool wall_found = false;
	float current_side_distance = 0.0;
	constexpr float max_distance = 100.0;

    while( !wall_found && current_side_distance < max_distance ) {

        walk_side = ( ray_length[x_dim] < ray_length[y_dim] ) ? x_dim : y_dim;

        current_side_distance = ray_length[walk_side];

        ray_length[walk_side] += step_size[walk_side];

        if( walk_side == x_dim )
            cell_to_test.x += cell_adjust.x;
        else
            cell_to_test.y += cell_adjust.y;

        wall_found = is_wall( cell_to_test );
    }

	if( wall_found ) {

        const Vec2 hit_point { player_position + ray_dir * current_side_distance };
        const Vec2 delta = hit_point - player_position;

        // credit: https://www.youtube.com/watch?v=eOCQfxRQ2pY
        const float distance = delta.x * std::cos( player_angle ) + delta.y * std::sin( player_angle );

        zoom_factor = distance * player_zoom;

        const Vec2 offset = hit_point - Vec2(cell_to_test);
        const Vec2 offset_corrected {
            (ray_dir.y < 0) ? 1 - offset.x : offset.x,
            (ray_dir.x > 0) ? 1 - offset.y : offset.y
        };

		cell_type = get_wall_texture_id( cell_to_test );
        wall_offset = ( walk_side == y_dim ) ? offset_corrected.x : offset_corrected.y;
    }

    return wall_found;
}

std::array<float, 2> WorldModel::calc_step_size( const Vec2& ray_dir )
{
    return std::array<float, 2>{
        std::sqrt( 1 + ( ray_dir.y / ray_dir.x ) * ( ray_dir.y / ray_dir.x ) ),
        std::sqrt( 1 + ( ray_dir.x / ray_dir.y ) * ( ray_dir.x / ray_dir.y ) )
    };
}

std::array<float, 2> WorldModel::calc_initial_ray_lengths( const Vec2& ray_start, const Vec2& ray_dir, const std::array<float, 2>& step_size )
{
	constexpr size_t x_dim = 0;
	constexpr size_t y_dim = 1;

    const Vec2 first_offset = ray_start - ray_start.floor();

    return std::array<float, 2> {
        ( ( ray_dir.x < 0 ) ? first_offset.x : ( 1.0F - first_offset.x ) ) * step_size[x_dim],
        ( ( ray_dir.y < 0 ) ? first_offset.y : ( 1.0F - first_offset.y ) ) * step_size[y_dim]
    };
}
