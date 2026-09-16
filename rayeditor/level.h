#pragma once

#include <vector>

#include "grid.h"

using Tile = int;

class Level
{
public:
    Level(int width, int height) : grid(width, height) {};
    
    Tile& tile(int x, int y) { return grid.cell(x, y); }
    const Tile& tile(int x, int y) const { return grid.cell(x, y); }

    size_t get_width() const { return grid.get_width(); }
    size_t get_height() const { return grid.get_height(); }
    const Grid<Tile>& get_grid() const { return grid; }

    bool contains( int x, int y ) { return grid.valid(x,y); }

    bool operator==(const Level& other) const { return grid == other.grid && player_position == other.player_position && player_angle == other.player_angle; };

    void set_player_origin( std::pair<float,float> origin ) { player_position = origin; };
    std::pair<float,float> get_player_origin() const { return player_position; }

    void set_player_angle( float angle ) { player_angle = angle; };
    float get_player_angle() const { return player_angle; }

private:
    Grid<Tile> grid;
    std::pair<float,float> player_position;
    float player_angle;

    friend std::ostream& operator<<( std::ostream& os, const Level& level );
    friend std::istream& operator>>( std::istream& is, Level& level );
};

inline std::ostream& operator<<( std::ostream& os, const Level& level )
{
    os << level.player_position.first << ' ' << level.player_position.second << ' ' << level.player_angle << '\n';
    os << level.grid;
    return os;
}

inline std::istream& operator>>( std::istream& is, Level& level )
{
    is >> level.player_position.first >> level.player_position.second >> level.player_angle;
    is >> level.grid;
    return is;
}
