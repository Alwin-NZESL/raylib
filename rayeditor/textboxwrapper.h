#pragma once

#include <string>
#include <array>

#include <raygui.h>
#include <algorithm>

class TextBoxWrapper
{
public:    
    TextBoxWrapper( Rectangle b, std::string l ) : bounds(b), label(l) {}

    void render_control()
    {
        GuiLabel({bounds.x, bounds.y, bounds.width, 20}, label.c_str() );

        if( GuiTextBox( {bounds.x, bounds.y + 30, bounds.width, 40}, content.data(), static_cast<int>(content.size()), edit_mode ) )
            edit_mode = !edit_mode;
    }

    std::string get_text() const { return std::string( content.data() ); }

    void set_text( std::string text )
    {
        const auto len = std::min(text.size(), content.size() - 1);
        std::copy_n(text.begin(), len, content.begin());
        content[len] = '\0';        
    }

private:
    std::array<char, 256> content = {};
    Rectangle bounds;
    std::string label;
    bool edit_mode = false;
};
