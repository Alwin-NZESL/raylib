/*
 * gamestate.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

class Renderer;
class Controls;

enum class GameStateCommand {
    NONE,
    SHOWTITLE,
    SHOWMENU,
    STARTPLAY,
    SHOWCREDITS,
    QUIT
};

class GameState {
public:
    GameState() {};
    virtual ~GameState() {};

    virtual GameStateCommand input( Controls& controls ) = 0;
    virtual void update() = 0;
    virtual void render( Renderer& renderer ) = 0;
};

class TitleState : public GameState {
public:
    GameStateCommand input( Controls& controls ) override;
    void update() override;
    void render( Renderer& renderer ) override;
};

class MenuState : public GameState {
public:
    GameStateCommand input( Controls& controls ) override;
    void update() override;
    void render( Renderer& renderer ) override;

private:
    int selected_option = 0;
};

class PlayState : public GameState {
public:
    GameStateCommand input( Controls& controls ) override;
    void update() override;
    void render( Renderer& renderer ) override;
};

class CreditsState : public GameState {
public:
    GameStateCommand input( Controls& controls ) override;
    void update() override;
    void render( Renderer& renderer ) override;
};
