#include "expired.h"
#include "Font.h"
#define ICON_FA_EXCLAMATION_TRIANGLE "\xef\x81\xb1"	// U+f071
#define ICON_FA_BOMB "\xef\x87\xa2"	// U+f1e2

namespace ExpiredSystem {
    std::atomic<bool> g_showExpired{false};
    std::atomic<bool> g_blinkState{true};
    static std::vector<Notification> notifications;
    static std::chrono::steady_clock::time_point g_expiredTime;
    
    bool IsExpired() {
        std::tm tm = {};
        tm.tm_year = 2025 - 1900;
        tm.tm_mon = 6; // August
        tm.tm_mday = 10;
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        
        std::time_t expired_time = std::mktime(&tm);
        return std::time(nullptr) > expired_time;
    }
    
    static void DrawBlinkingBackground(ImDrawList* draw, int width, int height) {
        static auto lastBlink = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() > 500) {
            g_blinkState = !g_blinkState;
            lastBlink = now;
        }
        
        ImU32 color = g_blinkState ? IM_COL32(255, 0, 0, 100) : IM_COL32(139, 0, 0, 150);
        draw->AddRectFilled(ImVec2(0, 0), ImVec2(width, height), color);
    }
    
    static void CreateNotification(int id, int width, int height) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> pos_x(50, width - 50);
        std::uniform_real_distribution<float> pos_y(50, height - 50);
        std::uniform_real_distribution<float> rot(-45, 45);
        std::uniform_real_distribution<float> scale(0.6f, 1.4f);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(id * 150));
        
        Notification n;
        n.pos = ImVec2(pos_x(gen), pos_y(gen));
        n.rot = rot(gen);
        n.scale = scale(gen);
        
        int colorSel = id % 4;
        switch(colorSel) {
            case 0: n.color = IM_COL32(255,255,255,255); break;
            case 1: n.color = IM_COL32(255,255,0,255); break;
            case 2: n.color = IM_COL32(255,100,100,255); break;
            default: n.color = IM_COL32(255,0,0,255); break;
        }
        
        notifications.push_back(n);
    }
    
    void StartNotificationThreads() {
        notifications.clear();
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back(CreateNotification, i, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        }
        
        for (auto& t : threads) t.join();
    }
    
    void Initialize() {
        if (IsExpired() && !g_showExpired) {
            g_showExpired = true;
            std::thread([]() { StartNotificationThreads(); }).detach();
        }
    }
    
    void Draw(ImDrawList* draw) {
        if (!g_showExpired) return;
        
        int width = ImGui::GetIO().DisplaySize.x;
        int height = ImGui::GetIO().DisplaySize.y;
        
        DrawBlinkingBackground(draw, width, height);
        
        // Main notification
        ImVec2 center(width/2.0f, height/2.0f);
        
        // Background
        draw->AddRectFilled(
            ImVec2(center.x - 380, center.y - 120),
            ImVec2(center.x + 380, center.y + 120),
            IM_COL32(0,0,0,220), 15.0f
        );
        
        // Border
        draw->AddRect(
            ImVec2(center.x - 380, center.y - 120),
            ImVec2(center.x + 380, center.y + 120),
            IM_COL32(255,0,0,255), 15.0f, 0, 3.0f
        );
        
        // Icons
        ImFont* font_big = ImGui::GetIO().Fonts->Fonts[0];
        draw->AddText(font_big, 48.0f, 
            ImVec2(center.x - 180, center.y - 80),
            IM_COL32(255,255,0,255), ICON_FA_EXCLAMATION_TRIANGLE);
        
        // Text
        draw->AddText(ImVec2(center.x - 120, center.y - 70), 
            IM_COL32(255,0,0,255), "Modz-Expired : 2025-08-10 00:00:00 !");
        draw->AddText(ImVec2(center.x - 120, center.y - 30), 
            IM_COL32(255,255,255,255), "Please download latest version:");
        draw->AddText(ImVec2(center.x - 120, center.y - 5), 
            IM_COL32(0,255,255,255), "https://t.me/+YrlwJfVtES0xMzE1");
        draw->AddText(ImVec2(center.x - 120, center.y + 20), 
            IM_COL32(255,255,0,255), "OR Chat admin @Pubbbbg997");
        
        // Random notifications
        for (const auto& n : notifications) {
            float size = 20.0f * n.scale;
            draw->AddText(ImVec2(n.pos.x - size/2, n.pos.y - size/2), 
                n.color, ICON_FA_BOMB);
            draw->AddText(ImVec2(n.pos.x - 25, n.pos.y + 10), 
                n.color, "EXPIRED!");
        }
    }
}