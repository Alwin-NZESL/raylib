/*
 * amazeditor.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "amazeditor.h"

#include <fstream>
#include <cmath>

void AMazEditor::process_input( const UICapture& input )
{
    if( input.type == UICapture::Type::LeftReleased )
        drop_spawn();

    if( !input.grid_coords )
        return;

    auto [x,y] = *input.grid_coords;

    if( input.type == UICapture::Type::LeftPressed )
        grab_spawn( x, y );

    if( input.type == UICapture::Type::LeftDown )
        paint_tile( x, y );

    if( input.type == UICapture::Type::RightPressed )
        erase_tile( x, y );
}

std::optional<EditorResult> AMazEditor::process_action( const EditorAction& actions )
{
    if( actions.action == EditorAction::Action::New )
        return new_level( actions.new_width, actions.new_height );

    if( actions.action == EditorAction::Action::Load )
        return load_level( actions.filename );

    if( actions.action == EditorAction::Action::Save )
        return save_level( actions.filename );

    if( actions.select_tile != -1 )
        selected_tile = actions.select_tile;

    return std::nullopt;
}

EditorResult AMazEditor::save_level( std::string filename )
{
    if( filename.empty() )
        return { EditorResult::OperationStatus::EmptyFilename };

    std::ofstream file(filename.c_str());
    if( !file )
        return { EditorResult::OperationStatus::OpenFailed, 0, 0 };

    file << level;
    file.flush();

    if( !file )
        return { EditorResult::OperationStatus::WriteFailed, 0, 0 };

    return { EditorResult::OperationStatus::SaveSuccess, 0, 0 };
}

EditorResult AMazEditor::load_level( std::string filename )
{
    if( filename.empty() )
        return { EditorResult::OperationStatus::EmptyFilename, 0, 0 };

    std::ifstream file(filename.c_str());
    if (!file)
        return { EditorResult::OperationStatus::OpenFailed, 0, 0 };

    Level loaded_level;

    if (!(file >> loaded_level))
        return { EditorResult::OperationStatus::ReadFailed, 0, 0 };

    if (loaded_level.get_width() < 5 || loaded_level.get_height() < 5)
        return { EditorResult::OperationStatus::FileMinExceeded, loaded_level.get_width(), loaded_level.get_height() };

    if (loaded_level.get_width() > 28 || loaded_level.get_height() > 28)
        return { EditorResult::OperationStatus::FileMaxExceeded, loaded_level.get_width(), loaded_level.get_height() };

    level = std::move(loaded_level);

    return { EditorResult::OperationStatus::LoadSuccess, level.get_width(), level.get_height() };
}

EditorResult AMazEditor::new_level( std::string width, std::string height )
{
    size_t new_width;
    size_t new_height;

    try {
        new_width = std::stoi( width );
        new_height = std::stoi( height );
    }
    catch( std::invalid_argument const& ex ) {
        return { EditorResult::OperationStatus::InvalidDimensions, 0, 0 };
    }
    catch( std::out_of_range const& ex ) {
        return { EditorResult::OperationStatus::InvalidDimensions, 0, 0 };
    }

    if( new_width < 5 || new_height < 5 )
        return { EditorResult::OperationStatus::MinExceeded, new_width, new_height };

    if( new_width > 28 || new_height > 28 )
        return { EditorResult::OperationStatus::MaxExceeded, new_width, new_height };

    level = Level( new_width, new_height );
    return { EditorResult::OperationStatus::NewSuccess, new_width, new_height };
}
    
void AMazEditor::grab_spawn( int x, int y )
{
    auto [spawn_x,spawn_y] = level.get_player_origin();

    if( level.contains( x, y ) &&  (x == std::floor(spawn_x)) && (y == std::floor(spawn_y)) )
         dragging = true;
}

void AMazEditor::drop_spawn( )
{
    if( !dragging )
        return;

    auto [spawn_x,spawn_y] = level.get_player_origin();

    level.tile( std::floor(spawn_x), std::floor(spawn_y) ) = 0;
    dragging = false;
}

void AMazEditor::paint_tile( int x, int y )
{
    if( !level.contains( x, y ) )
        return;

    if( dragging  )
        level.set_player_origin( {x,y} );
    else if( level.get_player_origin() != std::make_pair<float,float>( x, y) )
        level.tile(x, y) = selected_tile;      
}

void AMazEditor::erase_tile( int x, int y )
{
    if( level.contains( x, y ) )
        level.tile(x, y) = 0;
}
