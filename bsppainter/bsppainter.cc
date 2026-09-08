/*
 * bsppainter.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <memory>

#include "gamestate.h"
#include "renderer.h"
#include "controls.h"

class BSPPainter
{
public:
    BSPPainter();

    void run();
private:
    std::unique_ptr<GameState> currentState;
    Renderer renderer;
    Controls controls;

    bool handle_command( GameStateCommand command );
};

int main()
{
    BSPPainter app;
    app.run();
    return 0;
}

BSPPainter::BSPPainter()
{
    currentState = std::make_unique<TitleState>();
    renderer.init("BSP Painter", 800, 600, 60);
}

void BSPPainter::run()
{
    while( renderer.is_window_open() ) {

        GameStateCommand game_command = currentState->input( controls );
        if( game_command != GameStateCommand::NONE && handle_command( game_command ) )
            break;

        currentState->update();

        renderer.start_frame();

        currentState->render( renderer);

        renderer.end_frame();
    }
}

bool BSPPainter::handle_command( GameStateCommand command )
{
    switch( command ) {
    case GameStateCommand::NONE:
        break;
    case GameStateCommand::SHOWTITLE:
        currentState = std::make_unique<TitleState>();
        break;
    case GameStateCommand::SHOWMENU:
        currentState = std::make_unique<MenuState>();
        break;
    case GameStateCommand::STARTPLAY:
        currentState = std::make_unique<PlayState>();
        break;
    case GameStateCommand::SHOWCREDITS:
        currentState = std::make_unique<CreditsState>();
        break;
    case GameStateCommand::QUIT:
        return true;
    }
    return false;
}
