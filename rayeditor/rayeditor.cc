#include <fstream>
#include <array>
#include <exception>
#include <iostream>

#include <raylib.h>

#include "level.h"

std::array<Color, 10> palette = 
{
    WHITE,
    LIGHTGRAY,
    GRAY,
    DARKGRAY,
    YELLOW,
    GOLD,
    ORANGE,
    PINK,
    RED,
    MAROON
};

const size_t SQUARE_SIDE=20;

int main( int argc, char** argv )
{
    Level level( 24, 24 );
    if (argc > 1) {
        const std::string arg = argv[1];
        std::snprintf(filename.data(), filename.size(), "%s", arg.c_str());
    }

    InitWindow(1024, 800, "Ray Editor");

    SetTargetFPS(60);

    bool dragging = false;

    while( !WindowShouldClose() ) {

        auto origin = level.get_player_origin();

        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();

        int x = (mouse_x / SQUARE_SIDE) -1;
        int y = (mouse_y / SQUARE_SIDE) -1;

        if( level.contains( x, y ) ) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (x == std::floor(origin.first)) && (y == std::floor(origin.second)))
                dragging = true;

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if( dragging )
                    level.set_player_origin( {x,y} );
                else
                    level.tile(x, y) = selected_tile;      
            }      
        
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                level.tile(x, y) = 0;
        }

        if( dragging && IsMouseButtonReleased( MOUSE_BUTTON_LEFT) ) {
            level.tile(origin.first, origin.second) = 0;
            dragging = false;
        }

        BeginDrawing();
        ClearBackground( RED );

        for( size_t y = 0; y < level.get_height(); ++y )
            for( size_t x = 0; x < level.get_width(); ++x ) {
                DrawRectangle( (x+1) * SQUARE_SIDE, (y+1) * SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE, palette.at(level.tile(x,y)) );
                DrawRectangleLines( (x+1) * SQUARE_SIDE, (y+1) * SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE, BLACK );
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
