#include "custom.hpp"
#include "Colors.h"
#include "include/hpp/imgui.h"
#include <EGL/egl.h>
#include <string>
#include <Includes.h>
#include "obfuscate.h"
#include "expired.h"
android_app *g_App = 0;
int Width, Height;
using namespace ImGui;
using namespace std;
ImFont *G22;
ImFont* outfit_bold;
ImFont* outfit_medium;
const char* INI_PATH = "/data/data/" PACKAGE_NAME "/imgui/.ini";

#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/input.h>

#include "include/hpp/dlfcn.hpp"

#include "include/hpp/imgui/persistence.h"
#include "include/hpp/imgui/touch.h"
#include "include/sync_with_py.h"
#include "include/sync_entities.h"

#include "include/helpers.h"

#include "Iconcpp.h"
#include "Font.h"
#include "Bold.h"
#include "Medium.h"
#include "Icon.h"
#include "output.h"

static bool showKeyboard = false;

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <curl/curl.h>
#include <dlfcn.h>
#include <android/input.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <dobby.h>

// Deklarasi fungsi hook
int (*orig_ANativeWindow_getWidth)(ANativeWindow *window);
int (*orig_ANativeWindow_getHeight)(ANativeWindow *window);
int32_t (*orig_AInputQueue_getEvent)(AInputQueue *queue, AInputEvent **outEvent);

// Implementasi hook
int _ANativeWindow_getWidth(ANativeWindow *window) {
    return orig_ANativeWindow_getWidth ? orig_ANativeWindow_getWidth(window) : 0;
}

int _ANativeWindow_getHeight(ANativeWindow *window) {
    return orig_ANativeWindow_getHeight ? orig_ANativeWindow_getHeight(window) : 0;
}

int32_t hooked_AInputQueue_getEvent(AInputQueue *queue, AInputEvent **outEvent) {
    int32_t result = orig_AInputQueue_getEvent(queue, outEvent);
    if (result >= 0 && *outEvent != nullptr) {
        ImVec2 screen_scale = ImVec2(1.0f, 1.0f); // atau sesuaikan dengan DPI
        ImGui_ImplAndroid_HandleInputEvent(*outEvent, screen_scale);
    }
    return result;
}

// Fungsi bantu
static inline void *dlopen_ex(const char *name, int flags) {
    return dlopen(name, flags);
}

static inline void *dlsym_ex(void *handle, const char *symbol) {
    return dlsym(handle, symbol);
}

static inline void protectFunc(void *addr) {
    // Tambahkan proteksi memory jika perlu
    // Misal: mprotect(addr, size, PROT_READ | PROT_EXEC);
}

ImFont* combo_arrow;

// Custom Switch Function
bool CustomSwitch(const char* label, bool* v) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    
    const float height = 20.0f;
    const float width = 40.0f;
    const float radius = height * 0.50f;
    
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) { *v = !(*v); ImGui::MarkItemEdited(id); }

    const ImRect switch_bb(pos, pos + ImVec2(width, height));
    
    // Background
    ImU32 bg_col = *v ? IM_COL32(0, 255, 100, 255) : IM_COL32(100, 100, 100, 255);
    if (hovered) bg_col = *v ? IM_COL32(0, 200, 80, 255) : IM_COL32(120, 120, 120, 255);
    
    window->DrawList->AddRectFilled(switch_bb.Min, switch_bb.Max, bg_col, radius);
    
    // Knob
    float knob_pos = *v ? switch_bb.Max.x - radius - 2 : switch_bb.Min.x + radius + 2;
    ImVec2 knob_center(knob_pos, switch_bb.GetCenter().y);
    window->DrawList->AddCircleFilled(knob_center, radius - 2, IM_COL32(255, 255, 255, 255));
    
    // Label
    if (label_size.x > 0.0f) {
        ImVec2 text_pos(switch_bb.Max.x + style.ItemInnerSpacing.x, pos.y + style.FramePadding.y);
        window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), label);
    }

    return pressed;
}

#define MINIAUDIO_IMPLEMENTATION
//#include "miniaudio.h" //Audio
#include <iostream>
//#include "sound.h" //Audio Sound Array Byte 

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"
bool LoadTextureFromMemory(ImTextureID* out_texture, const stbi_uc* data, size_t size)
{
int image_width, image_height, channels;
stbi_uc* image_data = stbi_load_from_memory(data, size, &image_width, &image_height, &channels, STBI_rgb_alpha);   
if (image_data == nullptr)
return false;      
GLuint image_texture;
glGenTextures(1, &image_texture);
glBindTexture(GL_TEXTURE_2D, image_texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
stbi_image_free(image_data);
*out_texture = (ImTextureID)(intptr_t)image_texture;
return true;
}

#include "ImGui/IMAGES/telegramIMG.h"
ImTextureID telegramIMG;

#include "ImGui/IMAGES/infoIMG.h"
ImTextureID infoIMG;
#include "ImGui/IMAGES/nbackgroundIMG.h"
ImTextureID nbackgroundIMG;

#include "image jinx/jinx1/jinx1_0001.h"

struct TextureInfo {
    ImTextureID textureId;
    int DefineID;
    int x;
    int y;
    int w;
    int h;
};
static TextureInfo textureInfo;

TextureInfo CreateTexture(const unsigned char *buf,int len) {
    int w, h, n;
    stbi_uc *data = stbi_load_from_memory(buf, len, &w, &h, &n, 0);
    GLuint texture;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (n == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    stbi_image_free(data);
    textureInfo.textureId = reinterpret_cast<ImTextureID>((GLuint *) texture);
    textureInfo.w = w;
    textureInfo.h = h;
    return textureInfo;
}

TextureInfo jinx1_0001;

void LoadImages()
{    
LoadTextureFromMemory(&telegramIMG, telegramHEX, sizeof(telegramHEX));
LoadTextureFromMemory(&infoIMG, infoHEX, sizeof(infoHEX));
LoadTextureFromMemory(&nbackgroundIMG, nbackgroundHEX, sizeof(nbackgroundHEX));

jinx1_0001 = CreateTexture(jinx1_0001_data, sizeof(jinx1_0001_data));
}

static float holdTime = 0.0f;
static ImVec2 initialMousePos; 

//VECTOR TEXT
void AddTextWithFont(ImFont* font, float font_size, float x, float y, ImU32 color, const char* text) {
    GetWindowDrawList()->AddText(font, font_size, ImVec2(x, y), color, text);
}
//TEXT SIZE
float TextWidth(float font_size, ImFont* font, const char* text) {
    ImGui::PushFont(font);
    float text_width = ImGui::CalcTextSize(text).x * (font_size / ImGui::GetFontSize());
    ImGui::PopFont();
    return text_width;
}

#include <iostream>
#include <fstream>

struct {
    MemoryPatch Feature1;
} var;

int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;

void noMore(ImDrawList* draw) {
    ExpiredSystem::Initialize(); // Tambahkan ini
    ExpiredSystem::Draw(draw);
    if (ExpiredSystem::IsExpired()) return;

    auto entities = get_entities();
    
    int players = 0, bots = 0;
    for (const Entity& e : entities) {
        if (e.is_bot) bots++;
        else players++;

        if (!e.is_on_screen) continue;

        ImVec2 From;
        switch (sync_int["iESP_Point"]) {
            case 0: From = ImVec2(Width / 2, Height); break;
            case 1: From = ImVec2(Width / 2, Height / 2); break;
            case 2: From = ImVec2(Width / 2, 0); break;
        }

        if (sync_bool["bESP_Skeleton"]) {
            auto head = ToImVec2(e.bones.at("head"));
            auto spine = ToImVec2(e.bones.at("spine"));
            auto spine1 = ToImVec2(e.bones.at("spine1"));
            
            auto limbs_r_upperarm = ToImVec2(e.bones.at("limbs_r_upperarm"));
            auto limbs_r_forearm = ToImVec2(e.bones.at("limbs_r_forearm"));
            auto limbs_r_hand = ToImVec2(e.bones.at("limbs_r_hand"));
            
            auto limbs_r_thigh = ToImVec2(e.bones.at("limbs_r_thigh"));
            auto limbs_r_calf = ToImVec2(e.bones.at("limbs_r_calf"));
            
            auto limbs_l_upperarm = ToImVec2(e.bones.at("limbs_l_upperarm"));
            auto limbs_l_forearm = ToImVec2(e.bones.at("limbs_l_forearm"));
            auto limbs_l_hand = ToImVec2(e.bones.at("limbs_l_hand"));
            
            auto limbs_l_thigh = ToImVec2(e.bones.at("limbs_l_thigh"));
            auto limbs_l_calf = ToImVec2(e.bones.at("limbs_l_calf"));
            
            ImVec2 chest((limbs_r_upperarm.x + limbs_l_upperarm.x) / 2, (limbs_r_upperarm.y + limbs_l_upperarm.y) / 2);
            
            auto linecolor = e.is_visible ? colors["cESP_Skeleton"] : colors["cESP_SkeletonHidden"];
            auto thickness = sync_float["fESP_LineThickness"];
            
            draw->AddLine(head, chest, linecolor, thickness);
            draw->AddLine(chest, spine, linecolor, thickness);
            draw->AddLine(chest, limbs_r_upperarm, linecolor, thickness);
            draw->AddLine(chest, limbs_l_upperarm, linecolor, thickness);
            draw->AddLine(limbs_r_upperarm, limbs_r_forearm, linecolor, thickness);
            draw->AddLine(limbs_l_upperarm, limbs_l_forearm, linecolor, thickness);
            draw->AddLine(limbs_r_forearm, limbs_r_hand, linecolor, thickness);
            draw->AddLine(limbs_l_forearm, limbs_l_hand, linecolor, thickness);

            draw->AddLine(spine, limbs_l_thigh, linecolor, thickness);
            draw->AddLine(spine, limbs_r_thigh, linecolor, thickness);
            draw->AddLine(limbs_l_thigh, limbs_l_calf, linecolor, thickness);
            draw->AddLine(limbs_r_thigh, limbs_r_calf, linecolor, thickness);
        }

        ImVec2 To(ToImVec2(e.bones.at("head")));

        
        if (e.is_bot) {
            if (sync_bool["bESP_LineBots"]) draw->AddLine(From, To, e.is_visible ? colors["cESP_LineBots"] : colors["cESP_LineBotsHidden"], sync_float["fESP_LineThickness"]);
        } else {
            if (sync_bool["bESP_Line"]) draw->AddLine(From, To, e.is_visible ? colors["cESP_Line"] : colors["cESP_LineHidden"], sync_float["fESP_LineThickness"]);
        }
if (sync_bool["bAIM_SnapLine"]) {
    
// if (sync_bool["bAIM_SnapLine"] && e.is_snap) draw->AddLine(ImVec2(Width / 2, Height / 2), To, e.is_visible ? colors["cAIM_Line"] : colors["cAIM_LineHidden"], sync_float["fESP_LineThickness"]);

ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);
float aimFov = sync_float["fAimbotFov"]; // Misal: 50.0f
float maxLineDistance = aimFov; // Radius batas FOV (di layar)

// Hitung jarak ke crosshair (layar)
float dx = To.x - screenCenter.x;
float dy = To.y - screenCenter.y;
float distanceToCrosshair = sqrtf(dx * dx + dy * dy);

// Tampilkan garis hanya jika dalam FOV dan terlihat
if (e.is_visible && distanceToCrosshair <= maxLineDistance) {
    ImU32 aimColor = IM_COL32(255, 255, 0, 255); // Kuning terang
    float thickness = sync_float["fESP_LineThickness"];
//if (e.id == closestTarget.id && e.is_visible) {
    //draw->AddLine(screenCenter, To, IM_COL32(255, 50, 50, 255), sync_float["fESP_LineThickness"]);
//}

    draw->AddLine(screenCenter, To, aimColor, thickness);
}

}

if ((sync_bool["bESP_TeamID"] || sync_bool["bESP_Name"] || sync_bool["bESP_Distance"]) && !e.bones.empty()) {
    ImVec2 head = ToImVec2(e.bones.at("head"));
    ImVec2 foot_l = ToImVec2(e.bones.at("limbs_l_calf"));
    ImVec2 foot_r = ToImVec2(e.bones.at("limbs_r_calf"));
    ImVec2 feet = (foot_l.y > foot_r.y) ? foot_l : foot_r;

    float height = feet.y - head.y;
    float width = height * 0.6f;
    ImVec2 boxMin(head.x - width / 2, head.y);
    ImVec2 boxMax(head.x + width / 2, feet.y);
  //  ImColor fillColor = e.is_visible ? ImColor(0, 255, 0, 90) : ImColor(0, 255, 0, 40);
  //  draw->AddRectFilled(boxMin, boxMax, fillColor);

    float distance = 200.0f / height;

    std::string teamText = std::to_string(e.team_id);
    std::string distanceText = std::to_string((int)distance) + "m";

    ImVec2 teamSize = ImGui::CalcTextSize(teamText.c_str());
    ImVec2 distSize = ImGui::CalcTextSize(distanceText.c_str());

    float padding = 6.0f;
    float spacing = 10.0f;
    float totalWidth = padding;
    if (sync_bool["bESP_TeamID"])    totalWidth += teamSize.x + spacing;
    if (sync_bool["bESP_Distance"])  totalWidth += distSize.x;
    totalWidth += padding;

    float boxHeight = std::max(teamSize.y, distSize.y) + 4;

    ImVec2 boxPos = ImVec2(head.x - totalWidth / 2, head.y - boxHeight - 10);
    ImVec2 boxEnd = ImVec2(boxPos.x + totalWidth, boxPos.y + boxHeight);

     draw->AddRectFilled(boxPos, boxEnd, IM_COL32(0, 0, 0, 80), 3.0f);
    draw->AddRect(boxPos, boxEnd, IM_COL32(0, 255, 0, 255), 5.0f);

    float cursorX = boxPos.x + padding;
    if (sync_bool["bESP_TeamID"]) {
        draw->AddText(ImVec2(cursorX, boxPos.y + 2), IM_COL32(255, 255, 255, 255), teamText.c_str());
        cursorX += teamSize.x + spacing;
    }
    if (sync_bool["bESP_Distance"]) {
        draw->AddText(ImVec2(cursorX, boxPos.y + 2), IM_COL32(255, 255, 255, 255), distanceText.c_str());
    }

if (sync_bool["bESP_Name"]) {
    const char* tag = e.is_knocked ? "KNOCK" : (e.is_bot ? "BOT" : "PLAYER");
    ImVec2 tagSize = ImGui::CalcTextSize(tag);

    ImVec2 tagPos(feet.x - tagSize.x / 2, feet.y + 7); // 5px di bawah kaki
    ImU32 tagColor = e.is_knocked ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255);

    draw->AddRectFilled(ImVec2(tagPos.x - 4, tagPos.y - 2), ImVec2(tagPos.x + tagSize.x + 4, tagPos.y + tagSize.y + 2), IM_COL32(0, 0, 0, 80), 3.0f);
    draw->AddText(tagPos, tagColor, tag);
    }
}

if (sync_bool["bESP_Health"] && !e.bones.empty()) {

    ImVec2 head = ToImVec2(e.bones.at("head"));
    auto foot_l = ToImVec2(e.bones.at("limbs_l_calf"));
    auto foot_r = ToImVec2(e.bones.at("limbs_r_calf"));

    ImVec2 feet = (foot_l.y > foot_r.y) ? foot_l : foot_r;

    float height = feet.y - head.y;
    float width = height * 0.6f;

    ImVec2 boxMin(head.x - width / 2, head.y);
    ImVec2 boxMax(head.x + width / 2, feet.y);
    
    if (e.health > 0 && e.health < 100) {
        float barHeight = height;
        float barWidth = 4.0f;

        float healthRatio = e.health / 100.0f;
        float filledHeight = barHeight * healthRatio;

        ImVec2 barPos1(boxMin.x - 6.0f, boxMin.y); 
        ImVec2 barPos2(barPos1.x + barWidth, boxMax.y);

        ImVec2 fillPos1(barPos1.x, barPos2.y - filledHeight); 
        ImVec2 fillPos2(barPos2.x, barPos2.y);

        draw->AddRectFilled(barPos1, barPos2, IM_COL32(50, 50, 50, 200));
        draw->AddRectFilled(fillPos1, fillPos2, IM_COL32(0, 255, 0, 255));
    }
}

if (sync_bool["bESP_Box"] && !e.bones.empty()) {
    
    ImVec2 head = ToImVec2(e.bones.at("head"));
    auto foot_l = ToImVec2(e.bones.at("limbs_l_calf"));
    auto foot_r = ToImVec2(e.bones.at("limbs_r_calf"));

    
    ImVec2 feet = (foot_l.y > foot_r.y) ? foot_l : foot_r;

    
    float height = feet.y - head.y;
    float width = height * 0.6f;

    ImVec2 boxMin(head.x - width / 2, head.y);
    ImVec2 boxMax(head.x + width / 2, feet.y);

    ImColor borderColor = e.is_visible ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 0, 120);
    float thickness = sync_float["fESP_LineThickness"];


    draw->AddLine(boxMin, ImVec2(boxMin.x + 8, boxMin.y), borderColor, thickness);
    draw->AddLine(boxMin, ImVec2(boxMin.x, boxMin.y + 8), borderColor, thickness);

    draw->AddLine(ImVec2(boxMax.x, boxMin.y), ImVec2(boxMax.x - 8, boxMin.y), borderColor, thickness);
    draw->AddLine(ImVec2(boxMax.x, boxMin.y), ImVec2(boxMax.x, boxMin.y + 8), borderColor, thickness);

    draw->AddLine(ImVec2(boxMin.x, boxMax.y), ImVec2(boxMin.x + 8, boxMax.y), borderColor, thickness);
    draw->AddLine(ImVec2(boxMin.x, boxMax.y), ImVec2(boxMin.x, boxMax.y - 8), borderColor, thickness);

    draw->AddLine(boxMax, ImVec2(boxMax.x - 8, boxMax.y), borderColor, thickness);
    draw->AddLine(boxMax, ImVec2(boxMax.x, boxMax.y - 8), borderColor, thickness);
}


if (sync_bool["bESP_DangerCircle"] && e.is_visible < 15.0f) {
    draw->AddCircle(To, 30.0f, IM_COL32(255, 0, 0, 180), 32, 2.0f);
    draw->AddText(ImVec2(To.x - 20, To.y + 25), IM_COL32(255, 0, 0, 255), "DANGER!");
}

}

// PERKECIL PLAYER/BOT COUNTER
char buffer_p[16], buffer_b[16];
sprintf(buffer_p, "Player :%d", players);
sprintf(buffer_b, " Both :%d", bots);

std::string text_p = buffer_p;
std::string text_Name = "|";
std::string text_b = buffer_b;

ImVec2 size_p = CalcTextSize(text_p.c_str());
ImVec2 size_engine = CalcTextSize(text_Name.c_str());
ImVec2 size_b = CalcTextSize(text_b.c_str());

float spacing = 15.0f;
float total_width = size_p.x + spacing + size_engine.x + spacing + size_b.x;
ImVec2 pos = ImVec2(Width / 2 - total_width / 2, 50);
draw->AddRectFilled(ImVec2(pos.x - 10, pos.y - 2), ImVec2(pos.x + total_width + 20, pos.y + size_engine.y + 3), IM_COL32(0, 0, 0, 150));

float cursor_x = pos.x;
draw->AddText(nullptr, 24.0f, ImVec2(cursor_x, pos.y), IM_COL32(255, 255, 255, 255), text_p.c_str());
cursor_x += size_p.x + spacing;

draw->AddText(nullptr, 24.0f, ImVec2(cursor_x, pos.y), IM_COL32(0, 255, 255, 255), text_Name.c_str());
cursor_x += size_engine.x + spacing;

draw->AddText(nullptr, 24.0f, ImVec2(cursor_x, pos.y), IM_COL32(255, 255, 255, 255), text_b.c_str());

        if (!sync_bool["bAIM_NoTargetHideFov"] || (players || bots)) {
               
        if (sync_bool["bAIM_DrawFov"]) draw->AddCircle(ImVec2(Width / 2, Height / 2), sync_float["fAIM_Fov"], BLACK, 0, sync_float["fESP_LineThickness"]);
    } else { 
    }
}

static bool OnOff = true;
static bool abcd;
static bool showMenu = false;
static ImVec2 menuPos = ImVec2(100, 100); // Position for draggable menu toggle

namespace Settings {
static int Tab = 1;
}

void OptButton(const char* label, bool& value, float paddingX = -1, float paddingY = 28) {
    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 buttonSize = ImVec2(-1 , 28);

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 8.0f;

    if (value) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 255, 180));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 170, 255, 200));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 130, 255, 220));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 60, 60, 120));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(80, 80, 80, 150));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(100, 100, 100, 180));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    }

    if (ImGui::Button(label, buttonSize)) {
        value = !value;
    }

    ImGui::PopStyleColor(4);
}

// Custom Glow Slider
bool GlowSlider(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f") {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb)) return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    bool temp_input_is_active = false;
    bool value_changed = false;

    if (!temp_input_is_active) {
        const bool input_requested_by_tabbing = false;
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        if (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavInputId == id) {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
        }
    }

    if (g.ActiveId == id) {
        if (g.IO.MouseDown[0]) {
            const float mouse_abs_pos = g.IO.MousePos.x;
            float clicked_t = (frame_bb.Max.x - frame_bb.Min.x) > 0.0f ? ImClamp((mouse_abs_pos - frame_bb.Min.x) / (frame_bb.Max.x - frame_bb.Min.x), 0.0f, 1.0f) : 0.0f;
            float new_value = ImLerp(v_min, v_max, clicked_t);
            if (*v != new_value) {
                *v = new_value;
                value_changed = true;
            }
        } else {
            ImGui::ClearActiveID();
        }
    }

    // Render
    const float grab_t = (*v - v_min) / (v_max - v_min);
    const ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const ImU32 grab_col = IM_COL32(255, 255, 255, 255);
    const ImU32 glow_col = IM_COL32(255, 255, 255, 100);

    // Background
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);
    
    // Glow effect
    const float grab_pos = ImLerp(frame_bb.Min.x, frame_bb.Max.x, grab_t);
    const ImVec2 grab_center(grab_pos, frame_bb.GetCenter().y);
    window->DrawList->AddCircleFilled(grab_center, 8.0f, glow_col, 16);
    window->DrawList->AddCircleFilled(grab_center, 5.0f, grab_col, 16);

    // Value text
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *v);
    ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    // Label
    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

void DrawMenu(ImGuiIO &io) {
if (ExpiredSystem::IsExpired()) return;
/*static EGLSurface lastSurface = nullptr;
if (surface != lastSurface) {
    LOGI("New surface detected. Re-initializing ImGui.");
    lastSurface = surface;
    setup = false; // force re-init
}*/

static int CpLoper = 0;
std::vector<ImTextureID> ImageLogo;
ImageLogo.push_back((ImTextureID)jinx1_0001.textureId);

// DRAGGABLE TOGGLE BUTTON
ImGui::SetNextWindowPos(menuPos, ImGuiCond_FirstUseEver);
ImGui::SetNextWindowSize(ImVec2(120, 50), ImGuiCond_Always);

ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 200));
ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 150, 255, 255));

if (ImGui::Begin("##MenuToggle", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    menuPos = ImGui::GetWindowPos(); // Update position for dragging
    
    ImGui::SetCursorPos(ImVec2(10, 10));
    if (showMenu) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 50, 50, 180));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 70, 70, 200));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        if (ImGui::Button("HIDE", ImVec2(100, 30))) {
            showMenu = false;
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 255, 100, 180));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 255, 120, 200));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        if (ImGui::Button("SHOW", ImVec2(100, 30))) {
            showMenu = true;
        }
    }
    ImGui::PopStyleColor(3);
}
ImGui::End();
ImGui::PopStyleColor(2);
ImGui::PopStyleVar(2);

// MAIN MENU
if (showMenu) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x - 450, center.y - 350), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(15, 15, 15, 240));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 150, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(0, 100, 200, 255));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(0, 120, 220, 255));

    if (ImGui::Begin(OBFUSCATE("SpaceModz"), &showMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus)) {

        bool need_sync = false, tab_changed = false;
        static bool color_changed = false;

        sync_float["fHeight"] = Height;
        sync_float["fWidth"] = Width;

        static std::string current_tab = "";

        if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {

            // PROFESSIONAL TAB BUTTONS
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 5));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            
            ImGui::PushStyleColor(ImGuiCol_Tab, IM_COL32(40, 40, 40, 200));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, IM_COL32(0, 150, 255, 150));
            ImGui::PushStyleColor(ImGuiCol_TabActive, IM_COL32(0, 150, 255, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

            if (ImGui::BeginTabItem("INFO")) {
                Settings::Tab = 1;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("ESP FEATURES")) {
                Settings::Tab = 2;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("AIM SYSTEM")) {
                Settings::Tab = 3;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("MEMORY HACKS")) {
                Settings::Tab = 4;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("SETTINGS")) {
                Settings::Tab = 5;
                ImGui::EndTabItem();
            }

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(2);

            ImGui::Separator();

            // TAB CONTENT WITH PROFESSIONAL STYLING
            if (Settings::Tab == 1) { 
                ImGui::BeginChild("info_content", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                ImGui::PushFont(G22);
                ImGui::TextColored(ImVec4(0, 1, 0.5f, 1), "Welcome to Space Modz");
                ImGui::PopFont();
                
                ImGui::Separator();   
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Modz Information:");
                ImGui::Indent(20);
                ImGui::BulletText("Seller: @fork0xf5");
                ImGui::BulletText("Version: v3 Version");
                ImGui::BulletText("Build: Beta Edition");
                ImGui::Unindent(20);
                
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Feature Status:");  
                ImGui::Indent(20);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "ESP System - Safe");
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "AimBot System - Medium Risk");
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Silent Aim - High Risk");
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Memory Patches - Very High Risk");
                ImGui::Unindent(20);
                
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Usage Guidelines:");
                ImGui::Indent(20);
                ImGui::BulletText("Use ESP features for training purposes");
                ImGui::BulletText("Avoid using high-risk features in ranked matches");
                ImGui::BulletText("Always keep the mod updated");
                ImGui::BulletText("Report bugs to developer");
                ImGui::Unindent(20);
                
                ImGui::EndChild();
            }

            if (Settings::Tab == 2) { 
                if (current_tab != "esp") { current_tab = "esp"; tab_changed = true; }
                
                std::string label = std::string("ESP System Status: ") + (sync_bool["bESP"] ? "ACTIVE" : "INACTIVE");
                OptButton(label.c_str(), sync_bool["bESP"], -1, 35);
                
                ImGui::BeginChild("esp_content", ImVec2(0, 0), true);
                ImGui::Columns(2, "ESPColumns", true);
                ImGui::SetColumnOffset(1, 420);
                
                // LEFT COLUMN - FEATURES
                ImGui::BeginChild("esp_left", ImVec2(400, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                ImGui::TextColored(ImVec4(0, 1, 0.5f, 1), "Visual Features");
                ImGui::Separator();
                
                CustomSwitch("Player Line ESP", &sync_bool["bESP_Line"]);
                CustomSwitch("Bot Line ESP", &sync_bool["bESP_LineBots"]);
                CustomSwitch("Name Display", &sync_bool["bESP_Name"]);
                CustomSwitch("Distance Display", &sync_bool["bESP_Distance"]);
                CustomSwitch("Team ID Display", &sync_bool["bESP_TeamID"]);
                CustomSwitch("Skeleton ESP", &sync_bool["bESP_Skeleton"]);
                CustomSwitch("2D Box ESP", &sync_bool["bESP_Box"]);
                CustomSwitch("Health Bar", &sync_bool["bESP_Health"]);
                CustomSwitch("Danger Circle", &sync_bool["bESP_DangerCircle"]);
                
                ImGui::EndChild();
                
                // RIGHT COLUMN - SETTINGS
                ImGui::NextColumn();
                ImGui::BeginChild("esp_right", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Color Configuration");
                ImGui::Separator();
                
                ImGui::ColorEdit4("Player Line Color", &colors["cESP_Line"].Value.x, 
                    ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | 
                    ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar);

                ImGui::ColorEdit4("Hidden Player Color", &colors["cESP_LineHidden"].Value.x, 
                    ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | 
                    ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar);

                ImGui::Separator();
                
                ImGui::ColorEdit4("Skeleton Color", &colors["cESP_Skeleton"].Value.x, 
                    ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | 
                    ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar);

                ImGui::ColorEdit4("Hidden Skeleton Color", &colors["cESP_SkeletonHidden"].Value.x, 
                    ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | 
                    ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar);

                ImGui::Separator();

                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Line Origin Point");
                for (int i = 0; i < IM_ARRAYSIZE(espPoint); ++i) {
                    if (ImGui::RadioButton(espPoint[i], sync_int["iESP_Point"] == i)) {
                        sync_int["iESP_Point"] = i;
                        need_sync = true;
                    } 
                    if (i < IM_ARRAYSIZE(espPoint) - 1) ImGui::SameLine(); 
                }

                ImGui::Separator();
                GlowSlider("Line Thickness", &sync_float["fESP_LineThickness"], 1.0f, 10.0f, "%.1f");

                ImGui::EndChild();
                ImGui::Columns(1);
                ImGui::EndChild();
            }

            if (Settings::Tab == 3) { 
                if (current_tab != "aimbot") { current_tab = "aimbot"; tab_changed = true; }
                
                ImGui::BeginChild("aim_content", ImVec2(0, 0), true);
                ImGui::Columns(2, "AimColumns", true);
                ImGui::SetColumnOffset(1, 420);
                
                // LEFT COLUMN
                ImGui::BeginChild("aim_left", ImVec2(400, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Silent Aim System");
                ImGui::Separator();
                CustomSwitch("Enable Silent Aim", &sync_bool["bBulletTrack"]);
                ImGui::Spacing();
                CustomSwitch("Ignore Bots", &sync_bool["bBulletTrack_IgnoreBots"]);
                CustomSwitch("Ignore Knocked", &sync_bool["bBulletTrack_IgnoreKnocked"]);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1), "AimBot System");
                ImGui::Separator();
                CustomSwitch("Enable AimBot", &sync_bool["bAIM"]);
                ImGui::Spacing();
                CustomSwitch("Ignore Bots", &sync_bool["bAIM_IgnoreBots"]);
                CustomSwitch("Ignore Knocked", &sync_bool["bAIM_IgnoreKnocked"]);
                CustomSwitch("Visibility Check", &sync_bool["bAIM_CheckVisibility"]);
                
                ImGui::EndChild();

                // RIGHT COLUMN
                ImGui::NextColumn();
                ImGui::BeginChild("aim_right", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Visual Settings");
                ImGui::Separator();
                
                CustomSwitch("Show Target Line", &sync_bool["bAIM_SnapLine"]);
                GlowSlider("Target Line Range", &sync_float["fAimbotFov"], 50.0f, 100.0f, "%.1f");
                
                ImGui::Spacing();
                GlowSlider("Snap Strength", &sync_float["fAIM_SnapStrength"], 0.001f, 1.00f, "%.3f");
                
                ImGui::Spacing();
                CustomSwitch("Show FOV Circle", &sync_bool["bAIM_DrawFov"]);
                GlowSlider("FOV Circle Size", &sync_float["fAIM_Fov"], 10.0f, 300.0f, "%.1f");

                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Trigger Settings");
                
                ImGui::BeginTable("AimSettings", 2, ImGuiTableFlags_Borders);
                ImGui::TableSetupColumn("Trigger Mode");
                ImGui::TableSetupColumn("Target Bone");
                ImGui::TableHeadersRow();
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                for (int i = 0; i < IM_ARRAYSIZE(aimbotTrigger); ++i) {
                    if (ImGui::RadioButton(aimbotTrigger[i], sync_int["iAIM_Trigger"] == i)) {
                        sync_int["iAIM_Trigger"] = i;
                        need_sync = true;
                    } 
                }

                ImGui::TableNextColumn();
                for (int i = 0; i < IM_ARRAYSIZE(targetBone); ++i) {
                    if (ImGui::RadioButton(targetBone[i], sync_int["iAIM_TargetBone"] == i)) {
                        sync_int["iAIM_TargetBone"] = i;
                        need_sync = true;
                    } 
                }
                ImGui::EndTable();

                ImGui::EndChild();
                ImGui::Columns(1);
                ImGui::EndChild();
            }

            if (Settings::Tab == 4) { 
                if (current_tab != "Memory") { current_tab = "Memory";  tab_changed = true; }
                
                ImGui::BeginChild("memory_content", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "WARNING: Memory patches are high risk features!");
                ImGui::Separator();
                
                ImGui::BeginTable("MemoryFeatures", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                ImGui::TableSetupColumn("Feature", ImGuiTableColumnFlags_WidthFixed, 300);
                ImGui::TableSetupColumn("Risk Level", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                CustomSwitch("Sky Color Modification", &sync_bool["bFeature1"]);
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "LOW RISK");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                CustomSwitch("No Recoil", &sync_bool["bNoRecoil"]);
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "HIGH RISK");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                CustomSwitch("No Spread", &sync_bool["bNoSpread"]);
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "HIGH RISK");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                CustomSwitch("Skin Unlock", &sync_bool["SkinHack"]);
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "MEDIUM RISK");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                CustomSwitch("Wall Hack", &sync_bool["bXray"]);
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "VERY HIGH RISK");
                
                ImGui::EndTable();
                
                // Handle memory patches
                if (sync_bool["bFeature1"] && !last_sync_bool["bFeature1"]) { 
                    var.Feature1.Modify();   
                } else if (!sync_bool["bFeature1"] && last_sync_bool["bFeature1"]) { 
                    var.Feature1.Restore();   
                }
                
                last_sync_bool = sync_bool;
                
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Risk Information:");
                ImGui::BulletText("LOW RISK: Minimal detection chance");
                ImGui::BulletText("MEDIUM RISK: Moderate detection chance");
                ImGui::BulletText("HIGH RISK: High detection chance");
                ImGui::BulletText("VERY HIGH RISK: Almost certain detection");
                
                ImGui::EndChild();
            }

            if (Settings::Tab == 5) { 
                ImGui::BeginChild("settings_content", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                ImGui::TextColored(ImVec4(0, 1, 0.5f, 1), "Application Settings");
                ImGui::Separator();
                
                ImGui::BeginTable("AppInfo", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Application Name");
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 1, 1), "com.netease.newspike");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Version");
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 1, 1), " SAFE VERSION");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Build Date");
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 1, 1), "August 2025");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Developer");
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 1, 1), "Space Modz");
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Contact");
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0, 1, 1, 1), "Telegram: @SpaceModz");
                
                ImGui::EndTable();
                
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Performance Statistics");
                ImGui::Separator();
                
                ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);
                ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
                ImGui::Text("Screen Resolution: %dx%d", Width, Height);
                
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1), "Support Information:");
                ImGui::BulletText("For technical support, contact developer");
                ImGui::BulletText("Report bugs with detailed information");
                ImGui::BulletText("Feature requests are welcome");
                ImGui::BulletText("Keep the application updated");
                
                ImGui::EndChild();
            }

        } 
        EndTabBar();

        if (need_sync) sync_with_py();

    } 
    End();
    
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
}
}

static bool setup = false;
DEFINES(EGLBoolean, Draw, EGLDisplay dpy, EGLSurface surface) {
   // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &Width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &Height);
    
    if (Width <= 0 || Height <= 0) return _Draw(dpy, surface);
    
    if (!setup) {
        setup = true;
        CreateContext();

        ImGuiStyle& style = ImGui::GetStyle();

        // Professional styling
        style.Alpha = 1.0f;
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.WindowRounding = 12.0f;
        style.WindowBorderSize = 2.0f;
        style.WindowMinSize = ImVec2(32.0f, 32.0f);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.ChildRounding = 8.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 8.0f;
        style.PopupBorderSize = 1.0f;
        style.FramePadding = ImVec2(8.0f, 4.0f);
        style.FrameRounding = 8.0f;
        style.FrameBorderSize = 0.0f;
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.CellPadding = ImVec2(6.0f, 3.0f);
        style.IndentSpacing = 20.0f;
        style.ColumnsMinSpacing = 6.0f;
        style.ScrollbarSize = 16.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabMinSize = 12.0f;
        style.GrabRounding = 8.0f;
        style.TabRounding = 8.0f;
        style.TabBorderSize = 0.0f;
        style.TabMinWidthForCloseButton = 0.0f;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        LoadImages();       

        // Professional color scheme
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.00f, 0.59f, 1.00f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.59f, 1.00f, 0.67f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.39f, 0.78f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.47f, 0.86f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.78f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.80f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.47f, 0.86f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.59f, 1.00f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.59f, 1.00f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.59f, 1.00f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.59f, 1.00f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.59f, 1.00f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        ImGui_ImplOpenGL3_Init("#version 300 es");
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
        
        load_features();

        io.IniFilename = features["bImguiAutoSave"] ? INI_PATH : nullptr;
        
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = NULL;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF,
            0x2010, 0x205E,
            0x0600, 0x06FF,
            0xFE00, 0xFEFF,
            0,
        };
        static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
        ImFontConfig icons_config;

        ImFontConfig CustomFont;
        CustomFont.FontDataOwnedByAtlas = false;

        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.OversampleH = 2.5;
        icons_config.OversampleV = 2.5;

        io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 26.f, &CustomFont);

        io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 20.0f, &icons_config, icons_ranges);

        G22 = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 34.f, &CustomFont);

        ImFontConfig cfg;
        outfit_bold = io.Fonts->AddFontFromMemoryTTF(bold, sizeof(bold), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        outfit_medium = io.Fonts->AddFontFromMemoryTTF(medium, sizeof(medium), 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        cfg.SizePixels = ((float)density / 20.0f);
        io.Fonts->AddFontDefault(&cfg);

    }
    
    
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(Width, Height);
    NewFrame();
    
    ClearImGuiWindowTracking();
    
    ImDrawList* draw = ImGui::GetForegroundDrawList();
    noMore(draw);

    DrawMenu(io);
    
    EndFrame();
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
        // Pastikan ImGui selalu di-render terakhir, di atas layer game
    ImGui::GetIO().DeltaTime = 1.0f / 60.0f;          // stabilkan frame rate
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // supaya cursor game tetap
    glFlush();  // Paksa OpenGL selesaikan semua draw sebelum swap
    return _Draw(dpy, surface);
}

#define _GNU_SOURCE
#include <dlfcn.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#include "BYPASS.h"

void __IMGUI__() {
    start_entity_reader();

    void *libAndroid = dlopen_ex("libandroid.so", RTLD_LAZY);
    if (libAndroid) {
        void *symEvent1 = dlsym_ex(libAndroid, "ANativeWindow_getWidth");
        if (symEvent1) DobbyHook(symEvent1, (void *)_ANativeWindow_getWidth, (void **)&orig_ANativeWindow_getWidth);

        void *symEvent2 = dlsym_ex(libAndroid, "ANativeWindow_getHeight");
        if (symEvent2) DobbyHook(symEvent2, (void *)_ANativeWindow_getHeight, (void **)&orig_ANativeWindow_getHeight);

        void *symEvent = dlsym_ex(libAndroid, "AInputQueue_getEvent");
        if (symEvent) {
            protectFunc(symEvent);
            DobbyHook(symEvent, (void *)hooked_AInputQueue_getEvent, (void **)&orig_AInputQueue_getEvent);
        }
    }

    HOOKIT("libEGL.so", "eglSwapBuffers", Draw);
    LOGI("RETURNING FROM __IMGUI__");
}