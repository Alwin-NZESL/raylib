#pragma once

#include "messages.h"

namespace MockAdapter
{
    void reset();

    extern EditorAction::Action button_action;
    extern int selected_tile;
}