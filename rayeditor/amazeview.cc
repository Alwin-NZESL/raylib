/*
 * amazeui.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <unordered_map>

#define RAYGUI_IMPLEMENTATION

#include "amazeview.h"

void AMazeView::transform_coords( UICapture& capture )
{
    if( capture.x > grid_bounds.x && capture.y > grid_bounds.y )
        capture.grid_coords = std::make_pair( (capture.x - grid_bounds.x) / side_size, (capture.y - grid_bounds.y) / side_size );
}

const std::string& AMazeView::get_status_string( EditorResult::OperationStatus result)
{
    static const std::unordered_map<EditorResult::OperationStatus, std::string> FileResultStrings = {
        { EditorResult::OperationStatus::NewSuccess, "New level" },
        { EditorResult::OperationStatus::LoadSuccess, "Level loaded" },
        { EditorResult::OperationStatus::SaveSuccess, "Level saved" },
        { EditorResult::OperationStatus::EmptyFilename, "Enter a filename" },
        { EditorResult::OperationStatus::OpenFailed, "Unable to open file" },
        { EditorResult::OperationStatus::ReadFailed, "Unable to load level" },
        { EditorResult::OperationStatus::WriteFailed, "Unable to save level" },
        { EditorResult::OperationStatus::FileMinExceeded, "Invalid file: Minimum size is 5 x 5" },
        { EditorResult::OperationStatus::FileMaxExceeded, "Invalid file: Maximum size is 28 x 28" },
        { EditorResult::OperationStatus::InvalidDimensions, "Invalid dimensions" },
        { EditorResult::OperationStatus::MinExceeded, "Minimum size is 5 x 5" },
        { EditorResult::OperationStatus::MaxExceeded, "Maximum size is 28 x 28" },
    };

    return FileResultStrings.at(result);
}


void AMazeView::setup( const Level& level )
{
    static std::vector<std::pair<int,int>> styles {
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
    };

    for( auto style : styles )
        GuiSetStyle( DEFAULT, style.first, style.second );

    width.set_text( std::to_string( level.get_width() ) );
    height.set_text( std::to_string( level.get_height() ) );

    calc_side_size( level.get_width(), level.get_height() );
}

void AMazeView::calc_side_size( size_t width, size_t height )
{
    size_t horizontal_side_length = grid_bounds.width / width;
    size_t vertical_side_length = grid_bounds.height / height;

    side_size = (vertical_side_length < horizontal_side_length ) ? vertical_side_length : horizontal_side_length;
}

void AMazeView::render_grid( const Level& level )
{
    size_t grid_height = level.get_height();
    size_t grid_width = level.get_width();
    std::pair<float, float> origin = level.get_player_origin();

    for (size_t y = 0; y < grid_height; ++y)
        for (size_t x = 0; x < grid_width; ++x)
        {
            DrawRectangle( grid_bounds.x + x * side_size, grid_bounds.y + y * side_size, side_size, side_size, palette.at(level.tile(x, y)));
            DrawRectangleLines( grid_bounds.x + x * side_size, grid_bounds.y + y * side_size, side_size, side_size, BLACK);
        }

    DrawCircle( grid_bounds.x + (origin.first + .5) * side_size, grid_bounds.y + (origin.second + .5) * side_size, (side_size / 2) - 1, RED);
}

int AMazeView::render_toolbox( int selected_tile, Rectangle bounds )
{
    constexpr float tb_width = 150.0F;
    constexpr float tb_height = 38.0F;
    constexpr float tb_horizontal_spacing = 200.0F;
    constexpr float tb_vertical_spacing = 45.0F;

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

EditorAction AMazeView::render(  const Level& level, int selected_tile )
{
    EditorAction actions;

    render_grid( level );

    width.render_control();
    height.render_control();

    if( GuiButton({800, 60, 130, 40}, "New") ) {
        actions.action = EditorAction::Action::New;
        actions.new_width = width.get_text();
        actions.new_height = height.get_text();
    }

    filename.render_control();
    if( GuiButton({600, 230, 130, 40}, "Load") ) {
        actions.action = EditorAction::Action::Load;
        actions.filename = filename.get_text();
    }

    if( GuiButton({800, 230, 130, 40}, "Save") ) {
        actions.action = EditorAction::Action::Save;
        actions.filename = filename.get_text();
    }

    GuiLabel({600, 290, 330, 40}, status.c_str());

    actions.select_tile = render_toolbox( selected_tile, {600.0F, 350.0F, 0, 0});

    return actions;
}

void AMazeView::process( const EditorResult& update )
{
    status = get_status_string( update.result );

    if( update.result == EditorResult::OperationStatus::NewSuccess  || update.result == EditorResult::OperationStatus::LoadSuccess )
        calc_side_size( update.width, update.height );

    if( update.result == EditorResult::OperationStatus::LoadSuccess ) {
        width.set_text( std::to_string( update.width ) );
        height.set_text( std::to_string( update.height ) );
    }
}
