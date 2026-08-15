#include <raylib.h>
#include <iostream>
#include <cmath>
#include <vector>

void set_target( Vector2 * target )
{
    target->x = GetRandomValue( 5 * GetScreenWidth() / 8, 7 * GetScreenWidth()/8);
    target->y = GetRandomValue( GetScreenHeight() / 4, 3 * GetScreenHeight()/4); 
}


class Rocket
{
public:
    Rocket()
    {
        position = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
        velocity = {0, 0};
        for (size_t i = 0; i < 200; ++i)
        {
            Vector2 acceleration = { GetRandomValue(-100, 100) / 100.0f, GetRandomValue(-100, 100) / 100.0f };
            acceleration_dna.push_back(acceleration);
        }
    };

    void update( size_t step )
    {
        if( step >= acceleration_dna.size() ) { return; }
        velocity.x += acceleration_dna[step].x; velocity.y += acceleration_dna[step].y;
        position.x += velocity.x; position.y += velocity.y;

        position.x = ((int)position.x + GetScreenWidth())  % GetScreenWidth();
        position.y = ((int)position.y + GetScreenHeight()) % GetScreenHeight(); 
    }

    void draw()
    {
        Vector2 v1 = get_vertex(20, 0);
        Vector2 v2 = get_vertex(10, 120);
        Vector2 v3 = get_vertex(10, -120);

        DrawTriangle(v1, v2, v3, BLUE);
    }
private:    
    Vector2 position;
    Vector2 velocity;
    std::vector<Vector2> acceleration_dna;

    float to_radians(float degrees) { return degrees * (3.14159265f / 180.0f); }
    
    Vector2 get_vertex( float magnitude, float angle )
    {
        float direction = atan2f(velocity.y, velocity.x);
        return Vector2 {
            position.x + magnitude * cosf(to_radians(direction + angle)),
            GetScreenHeight() - (position.y + magnitude * sinf(to_radians(direction + angle)))
        };
    }
};

class Arena
{
public:
    Arena()
    {
        set_random_target();
        rockets.push_back(Rocket());
    }

    void update(size_t step)
    {
        update_target();
        update_rockets(step);
    }

    void draw()
    {
        draw_rockets();
        draw_target();
    }
private:
    Vector2 target;
    std::vector<Rocket> rockets;

    void draw_rockets()
    {
        for (auto& rocket : rockets)
            rocket.draw();
    }
    
    void draw_target( )
    {
        DrawCircleV(target, 10, RED);
    }

    void update_target()
    {
        if( IsKeyPressed(KEY_SPACE) ) { set_random_target(); }
    }

    void update_rockets( size_t step )
    {
        for (auto& rocket : rockets)
            rocket.update( step );
    }

    void set_random_target()
    {
        target.x = GetRandomValue( 5 * GetScreenWidth() / 8, 7 * GetScreenWidth()/8);
        target.y = GetRandomValue( GetScreenHeight() / 4, 3 * GetScreenHeight()/4); 
    }
};

int main()
{
    InitWindow(1024, 768, "Rocket Simulator");
    SetTargetFPS(60);

    Arena arena;

    size_t step = 0;

    while (!WindowShouldClose())
    {
        arena.update(step);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            arena.draw();
        }
        EndDrawing();

//        step = (step + 1) % 200;
        step = (step + 1);
    }

    CloseWindow();

    return 0;
}