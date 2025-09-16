#pragma once

#define  ImGui_DEFINE_MATH_OPERATORS
#include "ImGui/ImGui.h"
#include "ImGui/ImGui_internal.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <string>
using namespace std;

#define to_vec4( r, g, b, a ) ImColor( r / 255.f, g / 255.f, b / 255.f, a )

struct st_lua {

    float animation;
    bool pressed_whole, pressed_icon, active;
};

class c_custom {

public:
    bool checkbox( const char* label, bool* v, const char* hint = "", ImFont* font = ImGui::GetIO( ).Fonts->Fonts[0] );

int m_accent_color[4] = { 112,171,255, 255 };
    ImColor get_accent_color( float a = 1.f ) {

        return to_vec4( m_accent_color[0], m_accent_color[1], m_accent_color[2], a );
    }

};

inline c_custom custom;
