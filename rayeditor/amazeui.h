#pragma once

#include <string>

#include <raylib.h>

#include "level.h"
#include "messages.h"
#include "textboxwrapper.h"

class AMazeUI
{
public:    
    void setup( const Level& state );
    EditorActions render(  const Level& level, int selected_tile );
    void update( const LevelEditResult& update );

    void transform_coords( UICapture& capture );
    Rectangle get_grid_bounds() const { return grid_bounds; }
    size_t get_side_size() const { return side_size; }

private:
    size_t side_size = 0;
    std::string status;
    TextBoxWrapper filename{{600, 130, 330, 20}, "File name:"};
    TextBoxWrapper width{{600, 30, 80, 40}, "Width"};
    TextBoxWrapper height{{700, 30, 80, 40}, "Height"};

    const Rectangle grid_bounds{20, 20, 560, 560};
    const std::array<Color, 10> palette =  {
        WHITE, LIGHTGRAY, GRAY,
        DARKGRAY, YELLOW, GOLD,
        ORANGE, PINK, RED, MAROON
    };

    void render_grid( const Level& level );
    int render_toolbox( int selected_tile, Rectangle bounds );
    void calc_side_size( size_t width, size_t height );
    const std::string& get_status_string( LevelEditResult::OperationStatus result);
};
