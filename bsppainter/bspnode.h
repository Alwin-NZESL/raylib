/*
 * bspnode.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <raylib.h>
#include <utility>
#include <vector>
// #include <functional>

struct BSPNode
{
    BSPNode* front;
    BSPNode* back;
    std::pair<Vector2, Vector2> line;

    BSPNode() : front(nullptr), back(nullptr) {}
};

BSPNode * construct_tree( const std::vector<std::pair<Vector2, Vector2>>& lines );
void traverse_tree( BSPNode* node, Vector2& camera_pos, std::vector<std::pair<Vector2, Vector2>>& render_order);
void destruct_tree( BSPNode* node );
