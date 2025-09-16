#pragma once

#include <iostream>
#include <map>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "json.hpp"
using json = nlohmann::json;

#include "includes.h"

#define FIFO_PATH "/data/data/" PACKAGE_NAME "/imgui/sync"

static std::map<std::string, bool> sync_bool = {
    {"bAIM", false},
    {"bAIM_IgnoreBots", false},
    {"bAIM_CheckVisibility", true},
    {"bAIM_IgnoreKnocked", true},
    {"bAIM_SnapLine", false},
    {"bAIM_DrawFov", true},
    {"bAIM_NoTargetHideFov", true},
    {"bESP_DangerCircle", false},
    {"bESP_Distance", false},


    {"bESP", true},
    {"bESP_Line", false},
    {"bESP_LineBots", false},
    {"bESP_Skeleton", false},
    {"bESP_Box", false},
    {"bESP_Health", false},
    {"bESP_Name", false},
    {"bESP_Distance", false},
    {"bESP_TeamID", false},
    {"bESP_Knockesp", false},

    {"bBulletTrack", false},
    {"bBulletTrack_IgnoreKnocked", true},
    {"bBulletTrack_IgnoreBots", false},

    {"bXray", false},
    {"bSkinHack", false},
    {"bSpeed", false},
    {"bNoRecoil", false},
    {"bNoSpread", false},
    {"bFeature1", false},
};
static std::map<std::string, bool> last_sync_bool = sync_bool;
static std::map<std::string, float> sync_float = {
    {"fAIM_Fov", 100.0f},
    {"fESP_BoxRounding", 1.0f},
    {"fAimbotFov", 0.50},
    {"fSpeed", 1.0f},
    {"fESP_LineThickness", 2.0f},
    {"fAIM_SnapStrength", 0.1f},
    {"fBulletTrack_Probability", 70.0f},
    {"fHeight", 0.0f},
    {"fWidth", 0.0f}
};

static std::map<std::string, int> sync_int = {
    {"iAIM_TargetBone", 0},
    {"iAIM_Trigger", 0},
    {"iESP_Point", 0},
    {"iBulletTrack_TargetBone", 0},
};

static const char* targetBone[] = { "Head", "Chest" };
static const char* aimbotTrigger[] = { "Aim/Shoot", "Aim", "Shoot", "Always" };
static const char* espPoint[] = { "Bottom", "Mid", "Top" };

static void sync_with_py() {
    json j = json::object();
    
    for (const auto& pair : sync_bool) j[pair.first] = pair.second;
    for (const auto& pair : sync_float) j[pair.first] = pair.second;
    for (const auto& pair : sync_int) j[pair.first] = pair.second;
    
    std::string serialized = j.dump();
    
    int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fd >= 0) {
        write(fd, serialized.c_str(), serialized.length());
        write(fd, "\n", 1);
        close(fd);
        std::cout << "Sent to FIFO: " << serialized << std::endl;
    } else std::cerr << "Failed to open FIFO for writing, maybe no one's listening" << std::endl;
}
