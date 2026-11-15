/*
 * terain_generation.cc
 *
 * Based on the Procedural Map Generation video by Programming Chaos
 * (https://www.youtube.com/watch?v=gsG731XItNU&t=505s)
*/

#include <random>
#include <algorithm>
#include <array>
#include <vector>
#include <numeric>

#include <raylib.h>

constexpr int max_terain = 6;
constexpr int max_dir = 9;

using TerainType = int; 
using RulesType = std::array<std::array<TerainType, 2*max_terain>, 2*max_terain>;
using ColourType = std::array<std::array<float,3>, max_terain>;

constexpr RulesType constraints {{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, },
    { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, },
    { 0, 1, 2, 3, 3, 3, 3, 3, 3, 2, 1, 0, },
    { 0, 1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, },
    { 0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0, },
    { 0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0, },
    { 0, 1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, },
    { 0, 1, 2, 3, 3, 3, 3, 3, 3, 2, 1, 0, },
    { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }
}};

constexpr ColourType colours {{
    { 210.0, 1.0, 0.5}, /* dark blue (deep water)*/
    { 210.0, 1.0, 1.0}, /* light blue (shallow water)*/
    { 120.0, 1.0, 1.0}, /* light green (grass)*/
    { 120.0, 1.0, 0.3}, /* dark green (forrest)*/
    {   0.0, 0.3, 0.8}, /* light grey (hills)*/
    {   0.0, 0.0, 1.0}, /* white (mountains)*/
}};

class Map
{
public:
    Map( std::size_t width, std::size_t height, std::size_t cell_size, const RulesType& terrain_rules )
        : height(height), width(width), cell_size(cell_size), traversal_indeces(width * height)
    {
        std::random_device rd;
        gen = std::mt19937(rd());

        std::uniform_int_distribution<TerainType> rand_terain( 0, max_terain-1 );

        cells.reserve( width * height );
        for( std::size_t i = 0; i < width * height; ++i )
            cells.emplace_back( rand_terain(gen) );

        std::iota( traversal_indeces.begin(), traversal_indeces.end(), 0 );
        generate_rules( terrain_rules );

        for( TerainType terain = 0; terain < max_terain; ++terain ) {
            auto hsv_colour = colours.at(terain);
            colors[terain] = ColorFromHSV( hsv_colour.at(0), hsv_colour.at(1), hsv_colour.at(2) );
        }
    }

    void draw_terain( int posx, int posy, int radius, TerainType terain )
    {
        for( int x = -radius; x < radius; ++x )
            for( int y = -radius; y < radius; ++y )
                if( x * x + y * y < radius * radius )
                    if( posx + x >= 0 && posx + x < width && posy + y >= 0 && posy + y < height )
                        cells[ (posy + y) * width + (posx + x)] = terain;
    }

    std::size_t resolve_conflicts()
    {
        std::shuffle( traversal_indeces.begin(), traversal_indeces.end(), gen );

        std::size_t total_conflicts = std::accumulate( traversal_indeces.begin(), traversal_indeces.end(), 0, 
            [&](std::size_t accumulator, const std::size_t& idx)
            {
                std::size_t conflicts = count_conflicts( idx, cells[idx] );
                if( conflicts == 0 )
                    return accumulator;

                std::vector<TerainType> new_terain_options;

                for( TerainType new_terain = 0; new_terain < max_terain; ++new_terain )
                    if( count_conflicts( idx, new_terain ) <= conflicts )
                        new_terain_options.emplace_back(new_terain);

                std::uniform_int_distribution<std::size_t> rand_option( 0, new_terain_options.size()-1 );
                cells[idx] = new_terain_options[rand_option(gen)];

                return accumulator + 1;
            }
        );

        return total_conflicts;
    }

    std::size_t count_conflicts( const std::size_t idx, TerainType terain )
    {
        const std::size_t row { idx / width };
        const std::size_t col { idx % width };
        std::size_t conflicts {0};

        for( int dir = 0; dir < max_dir; ++dir ) {

            if( dir == 4 )
                continue;

            const auto& rule = rules.at(terain * max_dir + dir);

            int neighbor_row = ((row + (dir/3 - 1)) + height) % height; 
            int neighbor_col = ((col + (dir%3 - 1)) + width) % width;
            TerainType neighbour_terain = cells[ neighbor_row * width + neighbor_col];

            if( std::find( rule.begin(), rule.end(), neighbour_terain) == rule.end() )
                ++conflicts;
        }

        return conflicts;
    }

    void render()
    {
        for( std::size_t i = 0; i < width * height; ++i ) {

            std::size_t row = i / width;
            std::size_t col = i % width;
            TerainType terain = cells[i];

            DrawRectangle( col * cell_size, row * cell_size, cell_size, cell_size, colors[terain] );
        }
    }

    void render_target( int x, int y, int radius, TerainType terain )
    {
        DrawCircle( x, y, radius * cell_size, colors[terain]);
    }

private:
    std::size_t width;
    std::size_t height;
    std::size_t cell_size;
    std::vector<TerainType> cells;
    std::vector<std::size_t> traversal_indeces;
    std::array<std::vector<TerainType>, max_terain * max_dir> rules;
    std::array<Color, max_terain> colors;
    std::mt19937 gen;

    void generate_rules( const RulesType& terain_rules )
    {
        std::size_t max_row = terain_rules.size();
        std::size_t max_col = terain_rules[0].size();

        for( std::size_t row = 0; row < max_row; ++row ) {
            for( std::size_t col = 0; col < max_col; ++col ) {

                TerainType terain = terain_rules[row][col];

                for( std::size_t dir = 0; dir < max_dir; ++dir ) {

                    int neighbor_row = row + (dir/3 - 1); 
                    int neighbor_col = col + (dir%3 - 1);

                    if( neighbor_row >= 0 && neighbor_row < max_row && neighbor_col >= 0 && neighbor_col < max_col ) {

                        auto& rule = rules[terain * max_dir + dir];
                        TerainType neighbour_terain = terain_rules[neighbor_row][neighbor_col]; 

                        if( std::find(rule.begin(), rule.end(), neighbour_terain) == rule.end() )
                            rule.push_back( neighbour_terain );
                    }
                }
            }
        }
    }
};

class LogMessage
{
public:    
    LogMessage( std::size_t centre_x, std::size_t top_y ) : centre_x(centre_x), top_y(top_y) {}

    void render()
    {
        std::string log_message = "Iterations/Conflicts: " + std::to_string(iterations) + '/' + std::to_string(conflicts_remaining);

        int text_width = MeasureText(log_message.c_str(), 40);
        DrawText(log_message.c_str(), centre_x - text_width / 2, top_y, 40, WHITE);
    }

    void update( std::size_t conflicts )
    {
        conflicts_remaining = conflicts;
        if( conflicts_remaining )
            ++iterations;

    }

private:
    std::size_t centre_x;
    std::size_t top_y;
    std::size_t iterations;
    std::size_t conflicts_remaining;
};

int main( int argc, char * argv[] )
{
    constexpr std::size_t world_width = 400;
    constexpr std::size_t world_height = 250;
    constexpr std::size_t cell_size = 4;
    constexpr std::size_t pos_x = (world_width * cell_size) / 2;
    constexpr std::size_t pos_y = world_height * cell_size;

    Map map { world_width, world_height, cell_size, constraints };
    LogMessage log_message { pos_x, pos_y };
    std::size_t radius { 5 };
    TerainType draw_terain { 5 };

    InitWindow( world_width * cell_size, world_height * cell_size + 40, "Map generation" );
    SetTargetFPS( 60 );

    while( !WindowShouldClose() ) {

        int key = GetKeyPressed( );
        if( key >= KEY_ZERO && key < KEY_SIX)
            draw_terain = key - KEY_ZERO;
        if( key == KEY_UP )
            ++radius;
        if( key == KEY_DOWN && radius > 2 )
            --radius;

        if( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
            map.draw_terain( GetMouseX() / cell_size, GetMouseY() / cell_size, radius, draw_terain );
        else
            log_message.update( map.resolve_conflicts() );

        BeginDrawing();

            ClearBackground( BLACK );
            map.render();

            map.render_target( GetMouseX(), GetMouseY(), radius, draw_terain );

            log_message.render();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

