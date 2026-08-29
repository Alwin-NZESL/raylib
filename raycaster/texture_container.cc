/*
 * texture_container.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "texture_container.h"

#include "raylib.h"

const Vec2 TextureContainer::texture_scale = { 
    static_cast<float>(TextureContainer::TEXTURE_WIDTH), 
    static_cast<float>(TextureContainer::TEXTURE_HEIGHT)
};

TextureContainer::TextureContainer()
{
    make_generated_textures();
    load_textures();
}

void TextureContainer::load_textures()
{
    textures[8] = LoadImage("textures/redbrick.png");
    textures[9] = LoadImage("textures/purplestone.png");
    textures[10] = LoadImage("textures/greystone.png");
    textures[11] = LoadImage("textures/bluestone.png");
    textures[12] = LoadImage("textures/mossy.png");
    textures[13] = LoadImage("textures/wood.png");
    textures[14] = LoadImage("textures/colorstone.png");
    textures[15] = LoadImage("textures/eagle.png");

    for( int idx = 8; idx < TEXTURE_COUNT * 2; idx++ )
        ImageFormat(&textures[idx], PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);    
}

void TextureContainer::make_generated_textures()
{
    uint32_t * data[TEXTURE_COUNT];

    for( int idx = 0; idx < TEXTURE_COUNT; idx++ ) {

        textures[idx] = Image {
            .data = new uint32_t[TEXTURE_HEIGHT * TEXTURE_WIDTH],
            .width = TEXTURE_WIDTH,
            .height = TEXTURE_HEIGHT,
            .mipmaps = 1,
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
        };
        data[idx] = static_cast<uint32_t*>(textures[idx].data);
    }

    for( int x = 0; x < TEXTURE_WIDTH; x++ ) {
        for( int y = 0; y < TEXTURE_HEIGHT; y++ ) {

            uint8_t square_pattern = ((x>>1) ^ (y>>1)) << 2;
            uint8_t horz_gradient = y << 2;
            uint8_t diag_gradient = (y + x) << 1;
            uint8_t cross_pattern = (x != y && x != TEXTURE_WIDTH - y) ? 0xFE : 0x00;
            uint8_t brick_pattern = (x % 16 && y % 16) ? 0xC0 : 0x00;
            uint8_t halftonecolour = 0x80;
            uint8_t opaque = 0xFF;

            size_t index = y * TEXTURE_WIDTH + x;

            data[0][index] = (opaque << 24) + ( cross_pattern << 0) + (          0x00 << 8) + (          0x00 << 16); // flat red texture with black cross
            data[1][index] = (opaque << 24) + ( diag_gradient << 0) + ( diag_gradient << 8) + ( diag_gradient << 16); // sloped greyscale
            data[2][index] = (opaque << 24) + ( diag_gradient << 0) + ( diag_gradient << 8) + (          0x00 << 16); // sloped yellow gradient
            data[3][index] = (opaque << 24) + (square_pattern << 0) + (square_pattern << 8) + (square_pattern << 16); // xor greyscale
            data[4][index] = (opaque << 24) + (          0x00 << 0) + (square_pattern << 8) + (          0x00 << 16); // xor green
            data[5][index] = (opaque << 24) + ( brick_pattern << 0) + (          0x00 << 8) + (          0x00 << 16); // red bricks
            data[6][index] = (opaque << 24) + ( horz_gradient << 0) + (          0x00 << 8) + (          0x00 << 16); // red gradient
            data[7][index] = (opaque << 24) + (halftonecolour << 0) + (halftonecolour << 8) + (halftonecolour << 16); // flat grey texture
        }
    }
}

uint32_t TextureContainer::get_colour( int tex_id, Vec2 tex_coord ) const
{
    constexpr uint32_t black = (0xFF << 24);

    if( tex_id < 0 || tex_id >= textures.size() 
        || tex_coord.x < 0.0f || tex_coord.x >= 1.0f
        || tex_coord.y < 0.0f || tex_coord.y >= 1.0f
        || textures[tex_id].data == nullptr
    )
        return black;

    Vec2i coord{ tex_coord.scale( texture_scale ).floor() }; // Scale texture coordinates to pixel coordinates and floor to get integer pixel indices   
    
    size_t index = coord.y * TEXTURE_WIDTH + coord.x; // Compute pixel position in the image
    uint32_t* data = static_cast<uint32_t*>(textures[tex_id].data); // Get raw image data pointer

    return data[index];
}
