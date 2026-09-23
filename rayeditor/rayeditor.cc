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
#include <assert.h>
#include <algorithm>
#include <unordered_map>

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

enum class FileResult {
    LoadSuccess,
    SaveSuccess,
    EmptyFilename,
    OpenFailed,
    ReadFailed,
    WriteFailed
};

const std::string& get_fileresult_string( FileResult& result)
{
    static std::unordered_map<FileResult, std::string> FileResultStrings = {
        { FileResult::LoadSuccess, "Level loaded" },
        { FileResult::SaveSuccess, "Level saved" },
        { FileResult::EmptyFilename, "Enter a filename" },
        { FileResult::OpenFailed, "Unable to open file" },
        { FileResult::ReadFailed, "Unable to save level" },
        { FileResult::WriteFailed, "Unable to load level" },
    };

    return FileResultStrings[result];
}

struct TextBoxWrapper
{
    TextBoxWrapper( Rectangle b, std::string l ) : bounds(b), label(l) {}

    void paint_box()
    {
        GuiLabel({bounds.x, bounds.y, bounds.width, 20}, label.c_str() );

        if( GuiTextBox( {bounds.x, bounds.y + 30, bounds.width, 40}, content.data(), static_cast<int>(content.size()), edit_mode ) )
            edit_mode = !edit_mode;
    }

    std::string get_text() const { return std::string( content.data() ); }

    void set_text( std::string text )
    {
        const auto len = std::min(text.size(), content.size() - 1);
        std::copy_n(text.begin(), len, content.begin());
        content[len] = '\0';        
    }

    std::array<char, 256> content = {};
    Rectangle bounds;
    std::string label;
    bool edit_mode = false;
};

size_t calc_side_size( Rectangle bounds, Level &level )
{
    size_t horizontal_side_length = bounds.width / level.get_width();
    size_t vertical_side_length = bounds.height / level.get_height();

    return (vertical_side_length < horizontal_side_length ) ? vertical_side_length : horizontal_side_length;
}

FileResult save_file( std::string filename, Level &level )
{
    if (filename[0] == '\0') return FileResult::EmptyFilename;

    std::ofstream file(filename.data());
    if (!file) return FileResult::OpenFailed;

    if (!(file << level)) return FileResult::WriteFailed;

    return FileResult::SaveSuccess;
}

FileResult load_file( std::string filename, Level &level )
{
    if (filename[0] == '\0') return FileResult::EmptyFilename;

    std::ifstream file(filename.data());
    if (!file) return FileResult::OpenFailed;

    Level loaded_level;

    if (!(file >> loaded_level)) return FileResult::ReadFailed;

    level = std::move(loaded_level);
    return FileResult::LoadSuccess;
}

void paint_grid( Rectangle bounds, Level &level, std::pair<float, float> &origin, size_t side_size )
{
    size_t grid_height = level.get_height();
    size_t grid_width = level.get_width();

    for (size_t y = 0; y < grid_height; ++y)
        for (size_t x = 0; x < grid_width; ++x)
        {
            DrawRectangle( bounds.x + x * side_size, bounds.y + y * side_size, side_size, side_size, palette.at(level.tile(x, y)));
            DrawRectangleLines( bounds.x + x * side_size, bounds.y + y * side_size, side_size, side_size, BLACK);
        }

    DrawCircle( bounds.x + (origin.first + .5) * side_size, bounds.y + (origin.second + .5) * side_size, (side_size / 2) - 1, RED);
}

void paint_toolbox( Rectangle bounds, int &selected_tile )
{
    constexpr float tb_width = 150.0F;
    constexpr float tb_height = 38.0F;
    constexpr float tb_horizontal_spacing = 200.0F;
    constexpr float tb_vertical_spacing = 45.0F;

    for (int i = 0; i < 10; ++i)
    {
        bool active = (selected_tile == i);

        Rectangle toggle_bounds = { bounds.x + (i / 5) * tb_horizontal_spacing, bounds.y + (i % 5) * tb_vertical_spacing, tb_width, tb_height};

        GuiToggle(toggle_bounds, (i == 0) ? TextFormat("No wall") : TextFormat("Wall %d", i), &active);
        DrawRectangle((int)toggle_bounds.x + 8, (int)toggle_bounds.y + 8, 20, 20, palette.at(i));
        
        if (active)
            selected_tile = i;
    }
}

int main( int argc, char** argv )
{
    Level level;
    std::string status;
    int selected_tile = 1;
    TextBoxWrapper filename( {600, 130, 330, 20}, "File name:" );
    TextBoxWrapper width( {600, 30, 80, 40}, "Width" );
    TextBoxWrapper height( {700, 30, 80, 40}, "Height" );

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
    Rectangle grid_bounds{20, 20, 560, 560};

    size_t side_size = calc_side_size( grid_bounds, level );

    while( !WindowShouldClose() ) {

        auto origin = level.get_player_origin();

        int x = (GetMouseX() - grid_bounds.x) / side_size;
        int y = (GetMouseY() - grid_bounds.y) / side_size;

        if( level.contains( x, y ) ) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (x == std::floor(origin.first)) && (y == std::floor(origin.second)))
                dragging = true;

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if( dragging )
                    level.set_player_origin( {x,y} );
                else if( !((x == std::floor(origin.first)) && (y == std::floor(origin.second) )) )
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

        paint_grid( grid_bounds, level, origin, side_size);

        width.paint_box();
        height.paint_box();

        if( GuiButton({800, 60, 130, 40}, "New") ) {
            try {
                int new_width = std::stoi(width.get_text());
                int new_height = std::stoi(height.get_text());

                if( new_width < 5 || new_height < 5 ) {
                    status = "Minimum size is 5 x 5";
                } else if( new_width > 28 || new_height > 28 ) {
                    status = "Maximum size is 28 x 28";
                } else {
                    level = Level( new_width, new_height );
                    side_size = calc_side_size( grid_bounds, level );
                    status = "New level";
                }
            }
            catch(...) {
                status = "Invalid dimensions";
            }
        }

        filename.paint_box();

        if( GuiButton({600, 230, 130, 40}, "Load") ) {

            FileResult result = load_file( filename.get_text(), level );
            if( result == FileResult::LoadSuccess ) {
                width.set_text( std::to_string(level.get_width()) );
                height.set_text( std::to_string(level.get_height()) );
                side_size = calc_side_size( grid_bounds, level );
            }

            status = get_fileresult_string( result );
        }

        if( GuiButton({800, 230, 130, 40}, "Save") ) {

            FileResult result = save_file( filename.get_text(), level );
            
            status = get_fileresult_string( result );
        }

        GuiLabel({600, 290, 330, 40}, status.c_str());

        paint_toolbox( {600.0F, 350.0F, 0, 0}, selected_tile );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
