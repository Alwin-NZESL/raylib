/*
 * bspnode.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "bspnode.h"

#include <functional>

Vector2 operator-(const Vector2& a, const Vector2& b) { return { a.x - b.x, a.y - b.y }; }
auto cross_product = [](const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; };
auto lerp = [](const Vector2& a, const Vector2& b, float t) { return Vector2{ a.x + t * (b.x - a.x), a.y + t * (b.y - a.y) }; };

auto is_front = [](float side) { return side >= 0.0f; };
auto is_back = [](float side) { return side <= 0.0f; };

BSPNode * construct_tree( const std::vector<std::pair<Vector2, Vector2>>& lines )
{
    if( lines.empty() )
        return nullptr;

    BSPNode * node = new BSPNode();
    node->line = lines[0]; // for now, always pick the first one.

    std::vector<std::pair<Vector2, Vector2>> front_lines;
    std::vector<std::pair<Vector2, Vector2>> back_lines;

    for( size_t i = 1; i < lines.size(); ++i ) {

        const auto& line = lines[i];

        Vector2 node_dir = node->line.second - node->line.first;
        Vector2 start_vector = line.first - node->line.first;
        Vector2 end_vector = line.second - node->line.first;

        float start_side = cross_product( start_vector, node_dir );
        float end_side = cross_product( end_vector, node_dir );

        if( is_front(start_side) && is_front(end_side) )
            front_lines.emplace_back(line);
        else if( is_back(start_side) && is_back(end_side) )
            back_lines.emplace_back(line);
        else {
            // The line intersects the partitioning line, so we need to split the line
            float t = -start_side / (end_side - start_side);
            Vector2 intersection = lerp(line.first, line.second, t);

            if( start_side > 0 ) {
                front_lines.emplace_back(line.first, intersection);
                back_lines.emplace_back(intersection, line.second);
            } else {
                back_lines.emplace_back(line.first, intersection);
                front_lines.emplace_back(intersection, line.second);
            }
        }
    }

    node->front = construct_tree(front_lines);
    node->back = construct_tree(back_lines);

    return node;
}

void traverse_tree( BSPNode* node, Vector2& camera_pos, std::vector<std::pair<Vector2, Vector2>>& render_order)
{
    if (!node)
        return;

    // Determine which side of the partitioning line the camera is on
    Vector2 node_dir = node->line.second - node->line.first;
    Vector2 camera_vector = camera_pos - node->line.first;
    float camera_side = cross_product(camera_vector, node_dir);

    if (is_front(camera_side)) {
        traverse_tree(node->front, camera_pos, render_order);
        render_order.push_back(node->line);
        traverse_tree(node->back, camera_pos, render_order);
    } else {
        traverse_tree(node->back, camera_pos, render_order);
        render_order.push_back(node->line);
        traverse_tree(node->front, camera_pos, render_order);
    }
};

void destruct_tree(BSPNode* node)
{
    if (!node)
        return;

    destruct_tree(node->front);
    destruct_tree(node->back);

    delete node;
}

