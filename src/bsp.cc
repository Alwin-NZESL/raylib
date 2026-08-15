#include <raylib.h>
#include <utility>
#include <vector>
#include <functional>

struct BSPNode
{
    BSPNode* front;
    BSPNode* back;
    std::pair<Vector2, Vector2> line;

    BSPNode() : front(nullptr), back(nullptr) {}
};

Vector2 operator-(const Vector2& a, const Vector2& b) { return { a.x - b.x, a.y - b.y }; }
auto cross_product = [](const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; };
auto lerp = [](const Vector2& a, const Vector2& b, float t) { return Vector2{ a.x + t * (b.x - a.x), a.y + t * (b.y - a.y) }; };

auto is_front = [](float side) { return side >= 0.0f; };
auto is_back = [](float side) { return side <= 0.0f; };

BSPNode * construct_tree( const std::vector<std::pair<Vector2, Vector2>>& lines )
{
    if( lines.empty() )
        return nullptr;

    BSPNode * node = new BSPNode();
    node->line = lines[0]; // for now, always pick the first one.

    std::vector<std::pair<Vector2, Vector2>> front_lines;
    std::vector<std::pair<Vector2, Vector2>> back_lines;

    for( size_t i = 1; i < lines.size(); ++i ) {

        const auto& line = lines[i];

        Vector2 node_dir = node->line.second - node->line.first;
        Vector2 start_vector = line.first - node->line.first;
        Vector2 end_vector = line.second - node->line.first;

        float start_side = cross_product( start_vector, node_dir );
        float end_side = cross_product( end_vector, node_dir );

        if( is_front(start_side) && is_front(end_side) )
            front_lines.emplace_back(line);
        else if( is_back(start_side) && is_back(end_side) )
            back_lines.emplace_back(line);
        else {
            // The line intersects the partitioning line, so we need to split the line
            float t = -start_side / (end_side - start_side);
            Vector2 intersection = lerp(line.first, line.second, t);

            if( start_side > 0 ) {
                front_lines.emplace_back(line.first, intersection);
                back_lines.emplace_back(intersection, line.second);
            } else {
                back_lines.emplace_back(line.first, intersection);
                front_lines.emplace_back(intersection, line.second);
            }
        }
    }

    node->front = construct_tree(front_lines);
    node->back = construct_tree(back_lines);

    return node;
}

void traverse_tree( BSPNode* node, Vector2& camera_pos, std::vector<std::pair<Vector2, Vector2>>& render_order)
{
    if (!node)
        return;

    // Determine which side of the partitioning line the camera is on
    Vector2 node_dir = node->line.second - node->line.first;
    Vector2 camera_vector = camera_pos - node->line.first;
    float camera_side = cross_product(camera_vector, node_dir);

    if (is_front(camera_side)) {
        traverse_tree(node->front, camera_pos, render_order);
        render_order.push_back(node->line);
        traverse_tree(node->back, camera_pos, render_order);
    } else {
        traverse_tree(node->back, camera_pos, render_order);
        render_order.push_back(node->line);
        traverse_tree(node->front, camera_pos, render_order);
    }
};

void destruct_tree(BSPNode* node)
{
    if (!node)
        return;

    destruct_tree(node->front);
    destruct_tree(node->back);

    delete node;
}

int main( int argc, char* argv[] )
{
    std::vector<std::pair<Vector2, Vector2>> lines;
    Vector2 start_point;
    Vector2 end_point;
    Vector2 camera_pos;
    bool dragging = false;
    bool editMode = true;
    BSPNode* bsp_tree = nullptr;
    int count = 0;

    InitWindow(1024, 800, "BSP test");

    SetTargetFPS(60);

    while( !WindowShouldClose() ) {

        if( editMode ) {
            if( IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ) {
                end_point = start_point = GetMousePosition();
                dragging = true;
            }

            if( dragging ) {
                end_point = GetMousePosition();

                if( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ) {
                    lines.emplace_back(start_point, end_point);
                    dragging = false;
                }
            }

            if( IsKeyPressed(KEY_C) )
                lines.clear();

            if( IsKeyPressed(KEY_E) ) {
                editMode = false;
                count = 0;
                destruct_tree(bsp_tree);
                bsp_tree = construct_tree(lines);
                camera_pos = GetMousePosition();
            }

        } else {
            if( IsMouseButtonReleased(MOUSE_LEFT_BUTTON) )
                count++;

            if( IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) )
                count = 0; // Reset count when right mouse button is released in view mode

            if( IsKeyPressed(KEY_E) ) {
                editMode = true;
                dragging = false;
            }

            camera_pos = GetMousePosition(); // Using mouse position as camera for this example
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            if( editMode ) {

                for( const auto& line : lines )
                    DrawLineEx(line.first, line.second, 2.0f, BLACK);

                if( dragging )
                    DrawLineEx(start_point, end_point, 2.0f, RED);

                DrawText("Edit mode", 10, 10, 20, RED);

            } else {

                // Draw the lines in the order they would be rendered using the BSP tree
                std::vector<std::pair<Vector2, Vector2>> render_order;

                traverse_tree(bsp_tree, camera_pos, render_order);

                // Draw the lines in the calculated render order
                int i = 0;
                for (const auto& line : render_order) {
                    DrawLineEx(line.first, line.second, (i<count)? 4.0f : 2.0F, BLACK);
                    Vector2 midpoint = { (line.first.x + line.second.x) * 0.5f, (line.first.y + line.second.y) * 0.5f};
                    DrawText( TextFormat("%d", i), static_cast<int>(midpoint.x), static_cast<int>(midpoint.y), 20, RED );

                    ++i;
                }

                DrawCircleV(camera_pos, 6.0f, BLUE);

                DrawText("View mode", 10, 10, 20, RED);
            }

        EndDrawing();
    }

    CloseWindow();
}