#pragma once

#include "ImGui/imgui.h"
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <atomic>

namespace ExpiredSystem {
    extern std::atomic<bool> g_showExpired;
    extern std::atomic<bool> g_blinkState;
    
    struct Notification {
        ImVec2 pos;
        float rot;
        float scale;
        ImU32 color;
    };
    
    void Initialize();
    void Draw(ImDrawList* draw);
    bool IsExpired();
    void StartNotificationThreads();
}