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

enum class OperationStatus {
    LoadSuccess,
    SaveSuccess,
    EmptyFilename,
    OpenFailed,
    ReadFailed,
    WriteFailed,
    FileMinExceeded,
    FileMaxExceeded,
    NewSuccess,
    InvalidDimensions,
    MinExceeded,
    MaxExceeded
};

struct LevelEditResult
{
    OperationStatus result;
    std::string width;
    std::string height;
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

struct UIUpdate
{
    bool new_level = false;
    bool load_level = false;
    bool save_level = false;

    LevelEditResult new_result;
    LevelEditResult load_result;
    LevelEditResult save_result;

    size_t level_width;
    size_t level_height;
    std::string status;
};


const std::string& get_fileresult_string( OperationStatus result)
{
    static const std::unordered_map<OperationStatus, std::string> FileResultStrings = {
        { OperationStatus::NewSuccess, "New level" },
        { OperationStatus::LoadSuccess, "Level loaded" },
        { OperationStatus::SaveSuccess, "Level saved" },
        { OperationStatus::EmptyFilename, "Enter a filename" },
        { OperationStatus::OpenFailed, "Unable to open file" },
        { OperationStatus::ReadFailed, "Unable to load level" },
        { OperationStatus::WriteFailed, "Unable to save level" },
        { OperationStatus::FileMinExceeded, "Invalid file: Minimum size is 5 x 5" },
        { OperationStatus::FileMaxExceeded, "Invalid file: Maximum size is 28 x 28" },
        { OperationStatus::InvalidDimensions, "Invalid dimensions" },
        { OperationStatus::MinExceeded, "Minimum size is 5 x 5" },
        { OperationStatus::MaxExceeded, "Maximum size is 28 x 28" },
    };

    return FileResultStrings.at(result);
}


struct EditorState
{
    LevelEditResult new_level( std::string width, std::string height );
    LevelEditResult load_level( std::string filename );
    LevelEditResult save_level( std::string filename );

    void grab_spawn( int x, int y );
    void drop_spawn();

    void paint_tile( int x, int y );
    void erase_tile( int x, int y );

    Level level;
    int selected_tile = 1;
    bool dragging = false;
};

LevelEditResult EditorState::save_level( std::string filename )
{
    if( filename.empty() ) return { OperationStatus::EmptyFilename, "", "" };

    std::ofstream file(filename.c_str());
    if( !file ) return { OperationStatus::OpenFailed, "", "" };

    file << level;
    file.flush();

    if( !file ) return { OperationStatus::WriteFailed, "", "" };

    return { OperationStatus::SaveSuccess, "", "" };
}

LevelEditResult EditorState::load_level( std::string filename )
{
    if( filename.empty() )
        return { OperationStatus::EmptyFilename, "", "" };

    std::ifstream file(filename.c_str());
    if (!file)
        return { OperationStatus::OpenFailed, "", "" };

    Level loaded_level;

    if (!(file >> loaded_level))
        return { OperationStatus::ReadFailed, "", "" };

    if (loaded_level.get_width() < 5 || loaded_level.get_height() < 5)
        return { OperationStatus::FileMinExceeded, std::to_string(loaded_level.get_width()), std::to_string(loaded_level.get_height()) };

    if (loaded_level.get_width() > 28 || loaded_level.get_height() > 28)
        return { OperationStatus::FileMaxExceeded, std::to_string(loaded_level.get_width()), std::to_string(loaded_level.get_height()) };

    level = std::move(loaded_level);

    return { OperationStatus::LoadSuccess, std::to_string(level.get_width()), std::to_string(level.get_height()) };
}

LevelEditResult EditorState::new_level( std::string width, std::string height )
{
    try {

        int new_width = std::stoi( width );
        int new_height = std::stoi( height );

        if( new_width < 5 || new_height < 5 )
            return { OperationStatus::MinExceeded, width, height };

        if( new_width > 28 || new_height > 28 )
            return { OperationStatus::MaxExceeded, width, height };

        level = Level(new_width, new_height);
        return { OperationStatus::NewSuccess, "", "" };
    }
    catch (...) {
        return { OperationStatus::InvalidDimensions, width, height };
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

UIUpdate handle_actions( EditorState &editor, EditorActions actions )
{
    UIUpdate ret;

    if( actions.do_new_level ) {
        ret.new_level = true;

        ret.new_result = editor.new_level( actions.new_width, actions.new_height );
        if( ret.new_result.result == OperationStatus::NewSuccess ) {
            ret.level_width = editor.level.get_width();
            ret.level_height = editor.level.get_height();
        }
        ret.status = get_fileresult_string( ret.new_result.result );
    }

    if( actions.do_load_level ) {
        ret.load_level = true;

        ret.load_result = editor.load_level( actions.filename );
        if( ret.load_result.result == OperationStatus::LoadSuccess ) {
            ret.level_width = editor.level.get_width();
            ret.level_height = editor.level.get_height();
        }
        ret.status = get_fileresult_string( ret.load_result.result );
    }

    if( actions.do_save_level ) {
        ret.save_level = true;
        ret.save_result = editor.save_level( actions.filename );
        ret.status = get_fileresult_string( ret.save_result.result );
    }

    if( actions.select_tile != -1 )
        editor.selected_tile = actions.select_tile;

    return ret;
}

void update_ui( EditorUI& ui, UIUpdate& update )
{
    if( update.new_level ) {
        if( update.new_result.result == OperationStatus::NewSuccess )
            ui.calc_side_size( update.level_width, update.level_height );

        ui.status = update.status;
    }

    if( update.load_level ) {
        if( update.load_result.result == OperationStatus::LoadSuccess )
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






