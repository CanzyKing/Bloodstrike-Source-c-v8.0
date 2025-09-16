#include "custom.hpp"
#include <map>
using namespace ImGui;
bool c_custom::checkbox( const char* label, bool* v, const char* hint, ImFont* font ) {

    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID( label );
    const ImVec2 label_size = font->CalcTextSizeA( font->FontSize, FLT_MAX, 0, label );
    const ImVec2 hint_size = GetIO( ).Fonts->Fonts[ 0 ]->CalcTextSizeA( GetIO( ).Fonts->Fonts[ 0 ]->FontSize - 1.f, FLT_MAX, 0, hint );

    const float square_sz = 30.f;
    const float ROUNDING = 10.f;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect frame_bb( pos, pos + ImVec2( square_sz, square_sz ) );
    const ImRect total_bb( pos, pos + ImVec2( hint_size.x > label_size.x ? square_sz + hint_size.x + style.ItemInnerSpacing.x + 2 : square_sz + label_size.x + style.ItemInnerSpacing.x + 2, hint_size.x > 0 ? square_sz + 1 + hint_size.y : square_sz ) );
    ItemAdd( total_bb, id, &frame_bb );
    ItemSize( total_bb, style.FramePadding.y );

    bool hovered, held;
    bool pressed = ButtonBehavior( total_bb, id, &hovered, &held );
    if ( pressed )
    {
        *v = !( *v );
        MarkItemEdited( id );
    }

    static std::map< ImGuiID, float > values;
    auto value = values.find( id );

    if ( value == values.end( ) ) {

        values.insert( { id, 0.f } );
        value = values.find(id);
    }

    value->second = ImLerp( value->second, ( *v ? 1.f : 0.f ), 0.07f );
    RenderFrame( frame_bb.Min, frame_bb.Max, to_vec4( 255,255,255, style.Alpha ), 0, ROUNDING );
    window->DrawList->AddRect( frame_bb.Min, frame_bb.Max, to_vec4(255, 255, 255, style.Alpha), ROUNDING );

    RenderFrame( frame_bb.Min, frame_bb.Max, custom.get_accent_color( value->second * style.Alpha ), 0, ROUNDING );

    if (*v || value->second > 0.01f) {
    float t = value->second;
    ImVec2 center = frame_bb.GetCenter();
    float checkSize = 10.0f * t;

    ImVec2 p1 = ImVec2(center.x - checkSize * 0.5f, center.y);
    ImVec2 p2 = ImVec2(center.x - checkSize * 0.15f, center.y + checkSize * 2.5f);
    ImVec2 p3 = ImVec2(center.x + checkSize * 0.6f, center.y - checkSize * 2.5f);

    ImU32 checkColor = ImColor(0.0f, 0.0f, 0.0f, t * style.Alpha);
    window->DrawList->AddLine(p1, p2, checkColor, 5.0f);
    window->DrawList->AddLine(p2, p3, checkColor, 5.0f);
}



    ImVec2 label_pos = ImVec2( frame_bb.Max.x + style.ItemInnerSpacing.x + 2, frame_bb.GetCenter( ).y - label_size.y / 2 );
    if ( label_size.x > 0.f )
        window->DrawList->AddText( font, font->FontSize, label_pos, font == GetIO( ).Fonts->Fonts[1] ? to_vec4( 255, 255, 255, style.Alpha ) : to_vec4(226, 230, 233, style.Alpha ), label, FindRenderedTextEnd( label ) );

    if ( hint_size.x > 0.f )
        window->DrawList->AddText( GetIO( ).Fonts->Fonts[ 0 ], GetIO( ).Fonts->Fonts[ 0 ]->FontSize - 1.f, ImVec2( label_pos.x, label_pos.y + label_size.y + 1 ), GetColorU32( ImGuiCol_Text, 0.7f ), hint, FindRenderedTextEnd( hint ) );

    return pressed;
}