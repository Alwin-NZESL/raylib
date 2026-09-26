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
    bool do_new_level = false;
    bool do_load_level = false;
    bool do_save_level = false;

    int select_tile = -1;
    std::string filename;
    std::string new_width;
    std::string new_height;
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

