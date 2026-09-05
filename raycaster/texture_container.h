/*
 * texture_container.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <array>

#include "raylib.h"

#include "vec2.h"

class TextureContainer
{
public:
	TextureContainer();

	uint32_t get_colour( uint32_t* tex_buffer, Vec2 tex_coord ) const;
	uint32_t* get_buffer( int tex_id ) const;

private:
	const static int TEXTURE_WIDTH = 64;
	const static int TEXTURE_HEIGHT = 64;
	const static int TEXTURE_COUNT = 8;

	const static Vec2 texture_scale;

	std::array<Image, TEXTURE_COUNT * 2> textures;

	void load_textures();
	void make_generated_textures();
};
