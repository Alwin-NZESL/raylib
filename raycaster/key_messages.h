/*
 * key_messages.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <cstdint>

enum KeyState : uint16_t
{
    MoveForward = 1 << 0,
    MoveBackward = 1 << 1,
    RotateLeft = 1 << 2,
    RotateRight = 1 << 3,
    ZoomIn = 1 << 4,
    ZoomOut = 1 << 5,
    ToggleMinimap = 1 << 6,
    ToggleTextures = 1 << 7,
    ToggleDebugging = 1 << 8,
};

