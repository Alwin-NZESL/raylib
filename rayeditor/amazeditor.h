#pragma once

#include <string>
#include <optional>

#include "level.h"
#include "messages.h"

class AMazEditor
{
public:    
    void handle_input( const UICapture& input );
    std::optional<LevelEditResult> handle_actions( const EditorActions& actions );

    int get_selected_tile() const { return selected_tile; }
    const Level& get_level() const { return level; }

private:    
    Level level;
    int selected_tile = 1;
    bool dragging = false;

    LevelEditResult new_level( std::string width, std::string height );
    LevelEditResult load_level( std::string filename );
    LevelEditResult save_level( std::string filename );

    void grab_spawn( int x, int y );
    void drop_spawn();

    void paint_tile( int x, int y );
    void erase_tile( int x, int y );
};

