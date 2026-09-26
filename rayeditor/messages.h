/*
 * messages.h Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <string>
#include <optional>

struct UICapture
{
    enum class Type {
        LeftPressed,
        LeftDown,
        LeftReleased,
        RightPressed,
    };

    Type type;
    int x;
    int y;
    std::optional<std::pair<int, int>> grid_coords;
};

struct EditorActions
{
    enum class Action {
        None,
        New,
        Load,
        Save
    };

    Action action = Action::None;
    std::string filename;
    std::string new_width;
    std::string new_height;
    int select_tile = -1;
};

struct LevelEditResult
{
    enum class OperationStatus {
        LoadSuccess,
        SaveSuccess,
        EmptyFilename,
        OpenFailed,
        ReadFailed,
        WriteFailed,
        FileMinExceeded,
        FileMaxExceeded,
        NewSuccess,
        InvalidDimensions,
        MinExceeded,
        MaxExceeded
    };

    OperationStatus result;
    size_t width;
    size_t height;
};

