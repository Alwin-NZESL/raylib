/*
 * level.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <vector>

#include "grid.h"

using Tile = int;

class Level
{
public:
    Level(int width = 24, int height = 24) : grid(width, height) {};
    
    Tile& tile(int x, int y) { return grid.cell(x, y); }
    const Tile& tile(int x, int y) const { return grid.cell(x, y); }

    size_t get_width() const { return grid.get_width(); }
    size_t get_height() const { return grid.get_height(); }
    const Grid<Tile>& get_grid() const { return grid; }

    bool contains( int x, int y ) const { return grid.valid(x,y); }

    bool operator==(const Level& other) const { return grid == other.grid && player_position == other.player_position && player_angle == other.player_angle; };

    void set_player_origin( std::pair<float,float> origin ) { player_position = origin; };
    std::pair<float,float> get_player_origin() const { return player_position; }

    void set_player_angle( float angle ) { player_angle = angle; };
    float get_player_angle() const { return player_angle; }

private:
    Grid<Tile> grid;
    std::pair<float,float> player_position;
    float player_angle = 0.0;

    friend std::ostream& operator<<( std::ostream& os, const Level& level );
    friend std::istream& operator>>( std::istream& is, Level& level );
};

inline std::ostream& operator<<( std::ostream& os, const Level& level )
{
    size_t width = level.grid.get_width();
    size_t height = level.grid.get_height();

    os << width << " " << height << "\n";

    for (size_t  y = 0; y < height; ++y) {
        for (size_t  x = 0; x < width; ++x) {
            if( (level.player_position.first == x) && (level.player_position.second == y) )
                os << "P ";
            else
                os << level.grid.cell(x, y) << " ";
        }

        os << "\n";
    }
    
    return os;
}

inline std::istream& operator>>( std::istream& is, Level& level )
{
    size_t width, height;
    
    is >> width >> height;

    level.grid.clear();
    level.grid.resize(width, height);

    char ch;

    for (size_t  y = 0; y < height; ++y) {
        for (size_t  x = 0; x < width; ++x) {
            is >> ch;
            if( ch == 'P') {
                level.player_position = std::pair<float, float>( x, y );
                level.grid.cell(x, y) = 0;
            } else
                level.grid.cell(x, y) = (ch - '0');
        }
    }

    return is;
}
