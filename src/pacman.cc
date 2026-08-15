#include <cstdint>
#include <raylib.h>

#define CELL_SIZE 20

int main( int argc, char ** argv )
{
    bool cell[ 64 * 32];
    Vector2 cursor = {0,0};
    bool toggle_cell = false;

    Image background = LoadImage("../resources/background.png");
    uint32_t * pixels = (uint32_t *)background.data;
    for( size_t index = 0; index < 64 * 32; ++ index )
        cell[index] =  (pixels[index] == 0xFF000000 ? 1 : 0);


    InitWindow(64 * CELL_SIZE, 32 * CELL_SIZE, "Hello World");
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        toggle_cell = false;

        // Handle input  
        if( IsKeyPressed(KEY_A) ) { if( cursor.x > 0 ) --cursor.x; }
        if( IsKeyPressed(KEY_D) ) { if( cursor.x < 63 ) ++cursor.x; }
        if( IsKeyPressed(KEY_W) ) { if( cursor.y > 0 ) --cursor.y; }
        if( IsKeyPressed(KEY_S) ) { if( cursor.y < 31 ) ++cursor.y; }
        if( IsKeyPressed(KEY_SPACE) ) { toggle_cell = true; }

        // Update state
        if( toggle_cell )
            cell[ (int)cursor.y * 64 + (int)cursor.x] = !cell[ (int)cursor.y * 64 + (int)cursor.x];

        // Render
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for( size_t index = 0; index < 64 * 32; ++ index )
            DrawRectangle((index%64)*CELL_SIZE, (index/64)*CELL_SIZE, CELL_SIZE, CELL_SIZE, cell[index] ? BLACK : BLUE );

        DrawRectangle( cursor.x * CELL_SIZE, cursor.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN );

        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RED);

        EndDrawing();
    }

    CloseWindow();
}