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

    if( argc > 1 )    {

        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Unable to open level: " << argv[1] << '\n';
            return 1;
        }

        if (!(file >> level)) {
            std::cerr << "Unable to load level: " << argv[1] << '\n';
            return 1;
        }
    }

    InitWindow(1024, 800, "Ray Editor");

    SetTargetFPS(60);


    while( !WindowShouldClose() ) {

        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();

        int x = (mouse_x / SQUARE_SIDE) -1;
        int y = (mouse_y / SQUARE_SIDE) -1;

        if( level.contains( x, y ) ) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                level.tile(x, y) = (level.tile(x, y) + 1) % 10;
        
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                level.tile(x, y) = 0;
        }

        if( IsKeyPressed( KEY_S) ) {
            std::ofstream file(argv[1]);
            file << level;
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
