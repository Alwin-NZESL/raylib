/*
 * controls.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

 #include <cstdint>

 #include <raylib.h>

 #include "controls.h"

bool Controls::do_up() { return is_key_pressed( KEY_UP ); }
bool Controls::do_down() { return is_key_pressed( KEY_DOWN ); }
bool Controls::do_left() { return is_key_pressed( KEY_LEFT ); }
bool Controls::do_right() { return is_key_pressed( KEY_RIGHT ); }
bool Controls::do_accept() { return is_key_pressed( KEY_ENTER ) || is_key_pressed( KEY_SPACE ); }
bool Controls::do_cancel() { return is_key_pressed( KEY_ESCAPE ); }
bool Controls::do_quit() { return is_key_pressed( KEY_Q ) && is_key_down( KEY_LEFT_CONTROL ); }

bool Controls::is_key_pressed( int key ) { return IsKeyPressed( key ); }
bool Controls::is_key_released( int key ) { return IsKeyReleased( key ); }
bool Controls::is_key_down( int key ) { return IsKeyDown( key ); }
