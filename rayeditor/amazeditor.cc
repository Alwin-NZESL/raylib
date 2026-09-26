#include "amazeditor.h"

#include <fstream>
#include <cmath>

void AMazEditor::handle_input( const UICapture& input )
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

std::optional<LevelEditResult> AMazEditor::handle_actions( const EditorActions& actions )
{
    if( actions.do_new_level )
        return new_level( actions.new_width, actions.new_height );

    if( actions.do_load_level )
        return load_level( actions.filename );

    if( actions.do_save_level )
        return save_level( actions.filename );

    if( actions.select_tile != -1 )
        selected_tile = actions.select_tile;

    return std::nullopt;
}

LevelEditResult AMazEditor::save_level( std::string filename )
{
    if( filename.empty() )
        return { LevelEditResult::OperationStatus::EmptyFilename };

    std::ofstream file(filename.c_str());
    if( !file )
        return { LevelEditResult::OperationStatus::OpenFailed, 0, 0 };

    file << level;
    file.flush();

    if( !file )
        return { LevelEditResult::OperationStatus::WriteFailed, 0, 0 };

    return { LevelEditResult::OperationStatus::SaveSuccess, 0, 0 };
}

LevelEditResult AMazEditor::load_level( std::string filename )
{
    if( filename.empty() )
        return { LevelEditResult::OperationStatus::EmptyFilename, 0, 0 };

    std::ifstream file(filename.c_str());
    if (!file)
        return { LevelEditResult::OperationStatus::OpenFailed, 0, 0 };

    Level loaded_level;

    if (!(file >> loaded_level))
        return { LevelEditResult::OperationStatus::ReadFailed, 0, 0 };

    if (loaded_level.get_width() < 5 || loaded_level.get_height() < 5)
        return { LevelEditResult::OperationStatus::FileMinExceeded, loaded_level.get_width(), loaded_level.get_height() };

    if (loaded_level.get_width() > 28 || loaded_level.get_height() > 28)
        return { LevelEditResult::OperationStatus::FileMaxExceeded, loaded_level.get_width(), loaded_level.get_height() };

    level = std::move(loaded_level);

    return { LevelEditResult::OperationStatus::LoadSuccess, level.get_width(), level.get_height() };
}

LevelEditResult AMazEditor::new_level( std::string width, std::string height )
{
    size_t new_width;
    size_t new_height;

    try {
        new_width = std::stoi( width );
        new_height = std::stoi( height );
    }
    catch( std::invalid_argument const& ex ) {
        return { LevelEditResult::OperationStatus::InvalidDimensions, 0, 0 };
    }
    catch( std::out_of_range const& ex ) {
        return { LevelEditResult::OperationStatus::InvalidDimensions, 0, 0 };
    }

    if( new_width < 5 || new_height < 5 )
        return { LevelEditResult::OperationStatus::MinExceeded, new_width, new_height };

    if( new_width > 28 || new_height > 28 )
        return { LevelEditResult::OperationStatus::MaxExceeded, new_width, new_height };

    level = Level( new_width, new_height );
    return { LevelEditResult::OperationStatus::NewSuccess, new_width, new_height };
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
