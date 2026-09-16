/*
 * titlestate.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "gamestate.h"
#include "controls.h"

GameState::GameState()
{
    Image image = LoadImage("resources/tree.png");       // CPU-side
    texture = LoadTextureFromImage(image);
}

GameState::~GameState()
{
    UnloadTexture(texture);
}

GameStateCommand TitleState::input( Controls& controls )
{
    if( controls.do_accept() )
        return {GameStateCommand::SHOWMENU};

    return {GameStateCommand::NONE};
}

void TitleState::update( float elapsed_time )
{
}

void TitleState::render(Renderer& renderer)
{
    renderer.draw_texture(texture, 0, 0);
    renderer.text("Binary Space Partitioning Demo", 75, 200, 40, RED);
}

GameStateCommand CreditsState::input( Controls& controls )
{
    if( controls.do_accept() || controls.do_cancel() )
        return { GameStateCommand::QUIT };

    return { GameStateCommand::NONE };
}

void CreditsState::update( float elapsed_time )
{

}

void CreditsState::render( Renderer& renderer )
{
    renderer.draw_texture(texture, 0, 0);
    renderer.text("Credits", 50, 200, 30, YELLOW);
    renderer.text("Ramon Santamaria (Raylib)", 75, 240, 30, YELLOW);
    renderer.text("John Carmack (inspiration)", 75, 280, 30, YELLOW);
    renderer.text("Wikipedia (BSP page)", 75, 320, 30, YELLOW);
}
