#include <fstream>
#include <array>
#include <exception>
#include <iostream>
#include <string>
#include <cmath>

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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
    std::array<char, 256> filename = {};
    std::string status;
    bool filename_edit_mode = false;
    int selected_tile = 1;

    if (argc > 1) {
        const std::string arg = argv[1];
        std::snprintf(filename.data(), filename.size(), "%s", arg.c_str());
    }    

    InitWindow(1024, 600, "Ray Editor");

    // General text size
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Normal controls
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,   0xF0F0F0FF);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL,   0x356B50FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x183D2AFF);

    // Focused controls
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED,   0xFFFFFFFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED,   0x478C68FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0xA0D8B5FF);

    // Pressed controls
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED,   0xFFFFFFFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED,   0x244B38FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0xA0D8B5FF);    

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
        ClearBackground( DARKGREEN );

        for( size_t y = 0; y < level.get_height(); ++y )
            for( size_t x = 0; x < level.get_width(); ++x ) {
                DrawRectangle( (x+1) * SQUARE_SIDE, (y+1) * SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE, palette.at(level.tile(x,y)) );
                DrawRectangleLines( (x+1) * SQUARE_SIDE, (y+1) * SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE, BLACK );
            }

        GuiLabel({600, 30, 200, 20}, "File name:");

        if (GuiTextBox({600, 60, 330, 40},
                    filename.data(),
                    static_cast<int>(filename.size()),
                    filename_edit_mode)) {
            filename_edit_mode = !filename_edit_mode;
        }

        if (GuiButton({600, 130, 130, 40}, "Load")) {
            do {
                if (filename[0] == '\0') {
                    status = "Enter a filename";
                    break;
                }         

                std::ifstream file(filename.data());
                if (!file) {
                    status = "Unable to open file";
                    break;
                }

                Level loaded_level;

                if( !(file >> loaded_level) ) {
                    status = "Unable to load level";
                    break;
                }

                level = std::move( loaded_level );
                status = "Level loaded";
            } while(0);
        }

        if (GuiButton({800, 130, 130, 40}, "Save")) {
            do {
                if (filename[0] == '\0') {
                    status = "Enter a filename";
                    break;
                }         

                std::ofstream file(filename.data());

                if (!file) {
                    status = "Unable to open file";
                    break;
                }

                if( !(file << level) ) {
                    status = "Unable to save level";
                    break;
                }

                status = "Level saved";

            } while(0);
        }

        GuiLabel({600, 190, 330, 40}, status.c_str());
        
        for( int i = 0; i < 10; ++i ) {

            bool active = (selected_tile == i);

            Rectangle bounds = { 600.0F + (i/5) * 200.0F, 250.0F + (i%5) * 45.0F, 150.0F, 38.0F };

            if( i == 0 )
                GuiToggle(bounds, TextFormat("No wall"), &active);
            else
                GuiToggle(bounds, TextFormat("Wall %d", i), &active);

            if( active )
                selected_tile = i;

            DrawRectangle( (int)bounds.x + 8, (int)bounds.y + 8, 20, 20, palette.at(i) );
        }
        
        DrawCircle( (origin.first + 1.5) * SQUARE_SIDE, (origin.second + 1.5) * SQUARE_SIDE, (SQUARE_SIDE/2)-1, RED );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
