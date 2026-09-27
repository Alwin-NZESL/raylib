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

#include "amazeview.h"

#include <unordered_map>
#include <string>

#include "raylib_adapter.h"

namespace {
    constexpr float GridLeft   = 20.0F;
    constexpr float GridTop    = 20.0F;
    constexpr float GridSize   = 560.0F;
}

void AMazeView::setup( const Level& level )
{
    set_styles();

    width.set_text( std::to_string( level.get_width() ) );
    height.set_text( std::to_string( level.get_height() ) );

    calc_side_size( level.get_width(), level.get_height() );
}

EditorAction AMazeView::render(  const Level& level, int selected_tile )
{
    EditorAction actions;

    render_grid( level.get_grid(), side_size, level.get_player_origin(), GridLeft, GridTop );

    width.render_control();
    height.render_control();
    filename.render_control();
    message.render_label();

    actions.action = render_buttons();

    actions.new_width = width.get_text();
    actions.new_height = height.get_text();
    actions.filename = filename.get_text();
    actions.select_tile = render_toolbox( selected_tile );

    return actions;
}

void AMazeView::process( const EditorResult& update )
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

    message.set_label( FileResultStrings.at(update.result) );

    if( update.result == EditorResult::OperationStatus::NewSuccess  || update.result == EditorResult::OperationStatus::LoadSuccess )
        calc_side_size( update.width, update.height );

    if( update.result == EditorResult::OperationStatus::LoadSuccess ) {
        width.set_text( std::to_string( update.width ) );
        height.set_text( std::to_string( update.height ) );
    }
}

void AMazeView::transform_coords( UICapture& capture )
{
    if( capture.x > GridLeft && capture.y > GridTop )
        capture.grid_coords = std::make_pair( (capture.x - GridLeft) / side_size, (capture.y - GridTop) / side_size );
}

void AMazeView::calc_side_size( size_t width, size_t height )
{
    size_t horizontal_side_length = GridSize / width;
    size_t vertical_side_length = GridSize / height;

    side_size = (vertical_side_length < horizontal_side_length ) ? vertical_side_length : horizontal_side_length;
}
