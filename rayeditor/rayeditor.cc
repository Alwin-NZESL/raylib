#include <optional>

#include "messages.h"

#include <amazeditor.h>
#include <amazeui.h>

#include <raylib.h>

std::optional<UICapture> get_input()
{
    if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftPressed, GetMouseX(), GetMouseY() };

    if( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftDown, GetMouseX(), GetMouseY() };

    if( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
        return UICapture {UICapture::Type::RightPressed, GetMouseX(), GetMouseY() };

    if( IsMouseButtonReleased( MOUSE_BUTTON_LEFT) )
        return UICapture {UICapture::Type::LeftReleased, GetMouseX(), GetMouseY() };

    return std::nullopt;
}

int main(int argc, char **argv)
{
    AMazEditor editor;
    AMazeUI ui;

    InitWindow(1024, 600, "A-Maze-Thing Leveller");
    SetTargetFPS(60);

    ui.setup( editor.get_level() );

    while( !WindowShouldClose() ) {

        if( auto input = get_input() ) {
            ui.transform_coords( *input );
            editor.handle_input( *input );
        }

        BeginDrawing();

            ClearBackground(DARKGREEN);

            EditorActions actions = ui.render( editor.get_level(), editor.get_selected_tile() );

        EndDrawing();

        if( auto update = editor.handle_actions( actions ) )
            ui.update( *update );
    }

    CloseWindow();

    return 0;
}
