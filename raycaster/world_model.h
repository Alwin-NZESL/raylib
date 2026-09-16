/*
 * world_model.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <string>
#include <array>
#include <cstdint>

#include "vec2.h"
#include "level.h"

class WorldModel
{
public:
	void load_level( std::string filename );
	void handle_input();
	bool update( float elapsed_time );

	float angle_start() const { return player_angle - std::atan( player_zoom ); }
	float angle_step( float resolution ) const { return 2.0F * std::atan( player_zoom ) / resolution; }

	bool cast_ray( int step, int width, float& zoom_factor, int & cell_type, int& walk_side, double& wall_offset ) const;

	bool do_show_minimap() const { return show_minimap; }

	std::pair<int,int> get_background_ids( Vec2 hitpoint ) const;
	int get_celltype( Vec2i cell_to_test ) const { return level_data.tile( cell_to_test.x, cell_to_test.y ); }

	Vec2i get_world_dimension() const { return Vec2i( level_data.get_width(), level_data.get_height()); }
	Vec2 get_player_position() const { return player_position; }
	float get_player_angle() const { return player_angle; }
	float get_player_zoom() const { return player_zoom; }

private:
	enum KeyState : uint8_t
	{
		MoveForward = 1 << 0,
		MoveBackward = 1 << 1,
		RotateLeft = 1 << 2,
		RotateRight = 1 << 3,
		ZoomIn = 1 << 4,
		ZoomOut = 1 << 5,
		ToggleMinimap = 1 << 6,
		ToggleTextures = 1 << 7
	};

	Level level_data = Level(0,0);
	Vec2 player_position;
	float player_angle;
	float player_zoom = 0.4;
	uint8_t key_state = 0;
	bool show_minimap = true;
	bool show_generated_textures = false;

	bool is_wall( Vec2 position ) const;
	void key_state_action( KeyState key_state, bool is_pressed );
	int get_wall_texture_id( Vec2 hitpoint ) const;

	static std::array<float, 2> calc_step_size( const Vec2& ray_dir );
	static std::array<float, 2> calc_initial_ray_lengths( const Vec2& ray_start, const Vec2& ray_dir, const std::array<float, 2>& step_size );
};
