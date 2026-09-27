/*
 * ray_render.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ray_render.h"

#include <array>

#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

const std::array<Color, 10> palette =  {
    WHITE, LIGHTGRAY, GRAY,
    DARKGRAY, YELLOW, GOLD,
    ORANGE, PINK, RED, MAROON
};

void set_styles()
{
    const std::array<std::pair<int,int>, 10> styles { {
        { TEXT_SIZE, 20 },
        { TEXT_COLOR_NORMAL,   0xF0F0F0FF },  // Normal controls
        { BASE_COLOR_NORMAL,   0x356B50FF },
        { BORDER_COLOR_NORMAL, 0x183D2AFF },
        { TEXT_COLOR_FOCUSED,   0xFFFFFFFF }, // Focused controls
        { BASE_COLOR_FOCUSED,   0x478C68FF },
        { BORDER_COLOR_FOCUSED, 0xA0D8B5FF },
        { TEXT_COLOR_PRESSED,   0xFFFFFFFF }, // Pressed controls
        { BASE_COLOR_PRESSED,   0x244B38FF },
        { BORDER_COLOR_PRESSED, 0xA0D8B5FF },    
    }};

    for( const auto& style : styles )
        GuiSetStyle( DEFAULT, style.first, style.second );
}

void render_grid( const Grid<Tile>& grid, size_t side_size, std::pair<float, float> origin, float grid_left, float grid_top )
{
    for (size_t y = 0; y < grid.get_height(); ++y)
        for (size_t x = 0; x < grid.get_width(); ++x)
        {
            DrawRectangle( grid_left + x * side_size, grid_top + y * side_size, side_size, side_size, palette.at( grid.cell(x, y) ));
            DrawRectangleLines( grid_left + x * side_size, grid_top + y * side_size, side_size, side_size, BLACK);
        }

    DrawCircle( grid_left + (origin.first + .5) * side_size, grid_top + (origin.second + .5) * side_size, (side_size / 2) - 1, RED);
}

int render_toolbox( int selected_tile )
{
    constexpr float tb_width = 150.0F;
    constexpr float tb_height = 38.0F;
    constexpr float tb_horizontal_spacing = 200.0F;
    constexpr float tb_vertical_spacing = 45.0F;
    Rectangle bounds{600.0F, 350.0F, 0, 0};

    for( int i = 0; i < 10; ++i )
    {
        bool active = (selected_tile == i);

        Rectangle toggle_bounds = { bounds.x + (i / 5) * tb_horizontal_spacing, bounds.y + (i % 5) * tb_vertical_spacing, tb_width, tb_height};

        GuiToggle(toggle_bounds, (i == 0) ? TextFormat("No wall") : TextFormat("Wall %d", i), &active);
        DrawRectangle((int)toggle_bounds.x + 8, (int)toggle_bounds.y + 8, 20, 20, palette.at(i));
        
        if (active)
            selected_tile = i;
    }

    return selected_tile;
}

EditorAction::Action render_buttons()
{
    if( GuiButton({800, 60, 130, 40}, "New") )
        return EditorAction::Action::New;

    if( GuiButton({600, 230, 130, 40}, "Load") )
        return EditorAction::Action::Load;

    if( GuiButton({800, 230, 130, 40}, "Save") )
        return EditorAction::Action::Save;

    return EditorAction::Action::None;
}
