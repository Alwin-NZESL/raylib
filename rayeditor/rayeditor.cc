#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <utility>

#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include "level.h"

enum class FileResult {
    LoadSuccess,
    SaveSuccess,
    EmptyFilename,
    OpenFailed,
    ReadFailed,
    WriteFailed,
    MinExceeded,
    MaxExceeded
};

const std::string& get_fileresult_string( FileResult result)
{
    static const std::unordered_map<FileResult, std::string> FileResultStrings = {
        { FileResult::LoadSuccess, "Level loaded" },
        { FileResult::SaveSuccess, "Level saved" },
        { FileResult::EmptyFilename, "Enter a filename" },
        { FileResult::OpenFailed, "Unable to open file" },
        { FileResult::ReadFailed, "Unable to load level" },
        { FileResult::WriteFailed, "Unable to save level" },
        { FileResult::MinExceeded, "Invalid file: Minimum size is 5 x 5" },
        { FileResult::MaxExceeded, "Invalid file: Maximum size is 28 x 28" },
    };

    return FileResultStrings.at(result);
}

enum class NewLevelReturns
{
    Success,
    InvalidDimensions,
    MinExceeded,
    MaxExceeded,
};

const std::string& get_newlevelresult_string( NewLevelReturns result)
{
    static const std::unordered_map<NewLevelReturns, std::string> NewLevelResultStrings = {
        { NewLevelReturns::InvalidDimensions, "Invalid dimensions" },
        { NewLevelReturns::MinExceeded, "Minimum size is 5 x 5" },
        { NewLevelReturns::MaxExceeded, "Maximum size is 28 x 28" },
        { NewLevelReturns::Success, "New level" }
    };

    return NewLevelResultStrings.at(result);
}


struct LoadLevelReturns
{
    FileResult result;
    std::string width;
    std::string height;
};


struct EditorState
{
    NewLevelReturns new_level( std::string width, std::string height );
    LoadLevelReturns load_level( std::string filename );
    FileResult save_level( std::string filename );

    void grab_spawn( int x, int y );
    void drop_spawn();

    void paint_tile( int x, int y );
    void erase_tile( int x, int y );

    Level level;
    int selected_tile = 1;
    bool dragging = false;
};

FileResult EditorState::save_level( std::string filename )
{
    if( filename.empty() ) return FileResult::EmptyFilename;

    std::ofstream file(filename.c_str());
    if( !file ) return FileResult::OpenFailed;

    file << level;
    file.flush();

    if( !file ) return FileResult::WriteFailed;

    return FileResult::SaveSuccess;
}

LoadLevelReturns EditorState::load_level( std::string filename )
{
    if( filename.empty() )
        return { FileResult::EmptyFilename, "0", "0" };

    std::ifstream file(filename.c_str());
    if (!file)
        return { FileResult::OpenFailed, "0", "0" };

    Level loaded_level;

    if (!(file >> loaded_level))
        return { FileResult::ReadFailed, "0", "0" };

    if (loaded_level.get_width() < 5 || loaded_level.get_height() < 5)
        return { FileResult::MinExceeded, "0", "0" };

    if (loaded_level.get_width() > 28 || loaded_level.get_height() > 28)
        return { FileResult::MaxExceeded, "0", "0" };

    level = std::move(loaded_level);

    return { FileResult::LoadSuccess, std::to_string(level.get_width()), std::to_string(level.get_height()) };
}

NewLevelReturns EditorState::new_level( std::string width, std::string height )
{
    try {

        int new_width = std::stoi( width );
        int new_height = std::stoi( height );

        if( new_width < 5 || new_height < 5 )
            return NewLevelReturns::MinExceeded;

        if( new_width > 28 || new_height > 28 )
            return NewLevelReturns::MaxExceeded;

        level = Level(new_width, new_height);
        return NewLevelReturns::Success;
    }
    catch (...) {
        return NewLevelReturns::InvalidDimensions;
    }
}
    
void EditorState::grab_spawn( int x, int y )
{
    auto [spawn_x,spawn_y] = level.get_player_origin();

    if( level.contains( x, y ) &&  (x == std::floor(spawn_x)) && (y == std::floor(spawn_y)) )
         dragging = true;
}

void EditorState::drop_spawn( )
{
    if( dragging ) {
        auto [spawn_x,spawn_y] = level.get_player_origin();

        level.tile( std::floor(spawn_x), std::floor(spawn_y) ) = 0;
        dragging = false;
    }
}

void EditorState::paint_tile( int x, int y )
{
    if( !level.contains( x, y ) )
        return;

    if( dragging  )
        level.set_player_origin( {x,y} );
    else
        level.tile(x, y) = selected_tile;      
}

void EditorState::erase_tile( int x, int y )
{
    if( level.contains( x, y ) )
        level.tile(x, y) = 0;
}
    
struct TextBoxWrapper
{
    TextBoxWrapper( Rectangle b, std::string l ) : bounds(b), label(l) {}

    void render_control()
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

struct EditorActions
{
    bool do_new_level = false;
    bool do_load_level = false;
    bool do_save_level = false;

    int select_tile = -1;
    std::string filename;
    std::string new_width;
    std::string new_height;
};

struct EditorUI
{
    void setup( const Level& state );

    EditorActions render_controls( int selected_tile );
    void render_grid( const Level& level );
    int GUIToolBox( int selected_tile, Rectangle bounds );

    void calc_side_size( size_t width, size_t height );

    Rectangle grid_bounds{20, 20, 560, 560};

    size_t side_size = 0;
    std::string status;

    TextBoxWrapper filename{{600, 130, 330, 20}, "File name:"};
    TextBoxWrapper width{{600, 30, 80, 40}, "Width"};
    TextBoxWrapper height{{700, 30, 80, 40}, "Height"};
};

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

void EditorUI::setup( const Level& level )
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

    calc_side_size( level.get_width(), level.get_height() );
}

void EditorUI::calc_side_size( size_t width, size_t height )
{
    size_t horizontal_side_length = grid_bounds.width / width;
    size_t vertical_side_length = grid_bounds.height / height;

    side_size = (vertical_side_length < horizontal_side_length ) ? vertical_side_length : horizontal_side_length;
}

void EditorUI::render_grid( const Level& level )
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

int EditorUI::GUIToolBox( int selected_tile, Rectangle bounds )
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

EditorActions EditorUI::render_controls( int selected_tile )
{
    EditorActions actions;

    width.render_control();
    height.render_control();
    actions.do_new_level = GuiButton({800, 60, 130, 40}, "New");

    if( actions.do_new_level ) {
        actions.new_width = width.get_text();
        actions.new_height = height.get_text();
    }

    filename.render_control();
    actions.do_load_level = GuiButton({600, 230, 130, 40}, "Load");
    actions.do_save_level = GuiButton({800, 230, 130, 40}, "Save");

    if( actions.do_load_level || actions.do_save_level )
        actions.filename = filename.get_text();

    GuiLabel({600, 290, 330, 40}, status.c_str());

    actions.select_tile = GUIToolBox( selected_tile, {600.0F, 350.0F, 0, 0});

    return actions;
}

struct UIUpdate
{
    bool new_level = false;
    bool load_level = false;
    bool save_level = false;

    NewLevelReturns new_result;
    LoadLevelReturns load_result;
    FileResult save_result;

    size_t level_width;
    size_t level_height;
    std::string status;
};

// void handle_actions( EditorState &editor, EditorUI& ui, EditorActions actions )
// {
//     if( actions.do_new_level ) {

//         NewLevelReturns result = editor.new_level( actions.new_width, actions.new_height );
//         if( result == NewLevelReturns::Success )
//             ui.calc_side_size( editor.level.get_width(), editor.level.get_height() );

//         ui.status = get_newlevelresult_string( result );
//     }

//     if( actions.do_load_level ) {
//         LoadLevelReturns retvalue = editor.load_level( actions.filename );
//         if( retvalue.result == FileResult::LoadSuccess )
//         {
//             ui.width.set_text(retvalue.width);
//             ui.height.set_text(retvalue.height);
//             ui.calc_side_size( editor.level.get_width(), editor.level.get_height() );
//         }
//         ui.status = get_fileresult_string(retvalue.result);
//     }

//     if( actions.do_save_level ) {
//         FileResult result = editor.save_level( actions.filename );
//         ui.status = get_fileresult_string(result);
//     }

//     if( actions.select_tile != -1 )
//         editor.selected_tile = actions.select_tile;
// }

UIUpdate handle_actions( EditorState &editor, EditorActions actions )
{
    UIUpdate ret;

    if( actions.do_new_level ) {
        ret.new_level = true;

        ret.new_result = editor.new_level( actions.new_width, actions.new_height );
        if( ret.new_result == NewLevelReturns::Success ) {
            ret.level_width = editor.level.get_width();
            ret.level_height = editor.level.get_height();
        }
        ret.status = get_newlevelresult_string( ret.new_result );
    }

    if( actions.do_load_level ) {
        ret.load_level = true;

        ret.load_result = editor.load_level( actions.filename );
        if( ret.load_result.result == FileResult::LoadSuccess ) {
            ret.level_width = editor.level.get_width();
            ret.level_height = editor.level.get_height();
        }
        ret.status = get_fileresult_string(ret.load_result.result);
    }

    if( actions.do_save_level ) {
        ret.save_level = true;
        ret.save_result = editor.save_level( actions.filename );
        ret.status = get_fileresult_string(ret.save_result);
    }

    if( actions.select_tile != -1 )
        editor.selected_tile = actions.select_tile;

    return ret;
}

void update_ui( EditorUI& ui, UIUpdate& update )
{
    if( update.new_level ) {
        if( update.new_result == NewLevelReturns::Success )
            ui.calc_side_size( update.level_width, update.level_height );

        ui.status = update.status;
    }

    if( update.load_level ) {
        if( update.load_result.result == FileResult::LoadSuccess )
        {
            ui.width.set_text(update.load_result.width);
            ui.height.set_text(update.load_result.height);
            ui.calc_side_size( update.level_width, update.level_height );
        }
        ui.status = update.status;
    }

    if( update.save_level )
        ui.status = update.status;
}

void handle_grid_input( EditorState& editor, Rectangle grid_bounds, size_t side_size )
{
    if( GetMouseX() > grid_bounds.x && GetMouseY() > grid_bounds.y ) {

        int x = (GetMouseX() - grid_bounds.x) / side_size;
        int y = (GetMouseY() - grid_bounds.y) / side_size;

        if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )
            editor.grab_spawn( x, y );

        if( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
            editor.paint_tile( x, y );

        if( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
            editor.erase_tile( x, y );
    }

    if( IsMouseButtonReleased( MOUSE_BUTTON_LEFT) )
        editor.drop_spawn();
}

int main(int argc, char **argv)
{
    EditorState editor;
    EditorUI ui;

    InitWindow(1024, 600, "A-Maze-Thing Leveller");
    SetTargetFPS(60);

    ui.setup( editor.level );

    while( !WindowShouldClose() ) {

        handle_grid_input( editor, ui.grid_bounds, ui.side_size );

        BeginDrawing();

            ClearBackground(DARKGREEN);

            ui.render_grid( editor.level );

            EditorActions actions = ui.render_controls( editor.selected_tile );

        EndDrawing();

        UIUpdate update = handle_actions( editor, actions );

        update_ui( ui, update );
    }

    CloseWindow();

    return 0;
}






