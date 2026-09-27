
#include <unordered_map>

#include "key_messages.h"

#include <raylib.h>

uint16_t get_input( )
{
    uint16_t keystate = 0;

    static std::unordered_map<int, KeyState> key_map = {
        { KEY_UP, KeyState::MoveForward },
        { KEY_DOWN, KeyState::MoveBackward },
        { KEY_LEFT, KeyState::RotateLeft },
        { KEY_RIGHT, KeyState::RotateRight },
        { KEY_X, KeyState::ZoomIn },
        { KEY_Z, KeyState::ZoomOut },
    };

    static std::unordered_map<int, KeyState> key_map2 = {
        { KEY_SPACE, KeyState::ToggleMinimap },
        { KEY_T, KeyState::ToggleTextures },
        { KEY_D, KeyState::ToggleDebugging }
    };

    for( const auto &[key, key_state] : key_map ) {
        if( IsKeyDown(key) ) keystate |= key_state;
    }

    for( const auto &[key, key_state] : key_map2 ) {
        if( IsKeyPressed(key) ) keystate |= key_state;
        if( IsKeyReleased(key) ) keystate &= ~key_state;
    }

    return keystate;
}
