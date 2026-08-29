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

class WorldModel
{
public:
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

	void key_state_action( KeyState key_state, bool is_pressed );

	bool update( float elapsed_time );

	float angle_start() { return player_angle - std::atan( player_zoom ); }
	float angle_step( float resolution ) { return 2.0F * std::atan( player_zoom ) / resolution; }
	bool cast_ray( int step, int width, float& zoom_factor, int & cell_type, int& walk_side, double& wall_offset );

	bool do_show_minimap() { return show_minimap; }

	int get_floor_texture_id( Vec2 hitpoint );
	int get_ceiling_texture_id( Vec2 hitpoint );
	int get_wall_texture_id( Vec2 hitpoint );

	int get_celltype( Vec2i cell_to_test ) const { 
		return (level[ cell_to_test.x + cell_to_test.y * world_dimension.x ] - '0'); 
	}

	Vec2i get_world_dimension() { return world_dimension; }
	Vec2 get_player_position() { return player_position; }
	float get_player_angle() { return player_angle; }
	float get_player_zoom() { return player_zoom; }

private:
	uint8_t key_state = 0;
	bool show_minimap = true;
	bool show_generated_textures = false;
	Vec2i world_dimension{ 24, 24 };
	Vec2 player_position = { 2.0, 2.0 };
	float player_angle = 0.0;
	float player_zoom = 0.4;

	// std::string level = "111111111111111111111111"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"100000222220000303030001"
	// 					"100000200020000000000001"
	// 					"100000200020000300030001"
	// 					"100000200020000000000001"
	// 					"100000220220000303030001"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"100001020304050607080001"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"100000000000000000000001"
	// 					"144444444000000000000001"
	// 					"140400004000000000000001"
	// 					"140000504000000000000001"
	// 					"140400004000000000000001"
	// 					"140444444000000000000001"
	// 					"140000000000000000000001"
	// 					"144444444000000000000001"
	// 					"111111111111111111111111";

	std::string level = "444444444444444477777777"
						"400000000000000070000007"
						"401000000000000000000007"
						"402000000000000000000007"
						"403000000000000070000007"
						"404000055555555577077777"
						"405000050505050570007771"
						"406000050000000570000008"
						"407000000000000000007771"
						"408000050000000570000008"
						"400000050000000570007771"
						"400000055550555577777771"
						"666666666660666666666666"
						"800000000000000000000004"
						"666666066660666666666666"
						"444444044460622222223333"
						"400000000460620000020002"
						"400000000000620050020002"
						"400000000460620000022022"
						"406060000460000050000002"
						"400500000460620000022022"
						"406060000460620050020002"
						"400000000460620000020002"
						"444444444411122222233333";

	bool is_wall( Vec2 position );

	static std::array<float, 2> calc_step_size( const Vec2& ray_dir );
	static std::array<float, 2> calc_initial_ray_lengths( const Vec2& ray_start, const Vec2& ray_dir, const std::array<float, 2>& step_size );
};
