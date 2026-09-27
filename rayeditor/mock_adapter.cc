/*
 * mock_adapter.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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


#include "adapter.h"

namespace MockAdapter
{
    EditorAction::Action button_action = EditorAction::Action::None;
    int selected_tile = 1;

    void reset() {
        button_action = EditorAction::Action::None;
        selected_tile = 1;
    }
}

std::optional<UICapture> get_input()
{
    return std::nullopt;
}

void set_styles()
{
}

void render_grid( const Grid<Tile>& grid, size_t side_size, std::pair<float, float> origin, float grid_left, float grid_top )
{
}

int render_toolbox( int selected_tile )
{
    return MockAdapter::selected_tile;
}

EditorAction::Action render_buttons()
{
    return MockAdapter::button_action;
}
