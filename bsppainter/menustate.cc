/*
 * MenuState.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <format>

#include "gamestate.h"
#include "renderer.h"
#include "controls.h"

GameStateCommand MenuState::input( Controls& controls )
{
    if( controls.do_up() )
    {
        if( --selected_option < 0 )
            selected_option = 0;
    }
    else if( controls.do_down() )
    {
        if( ++selected_option > 4 )
            selected_option = 4;
    }
    else if( controls.do_cancel() )
        return GameStateCommand::SHOWTITLE;

    else if( controls.do_accept() ) {
        if( selected_option == 4 )
            return GameStateCommand::SHOWCREDITS;
        else
            return GameStateCommand::STARTPLAY;
    }

    return GameStateCommand::NONE;
}

void MenuState::update()
{
}

void MenuState::render(Renderer& renderer)
{
    renderer.clear(RAYWHITE);
    renderer.text(std::format("Menu screen: option {}", selected_option).c_str(), 10, 10, 20, DARKGRAY);
}
