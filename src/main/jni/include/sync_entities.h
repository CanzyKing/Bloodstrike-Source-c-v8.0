#pragma once

#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <sstream>
#include <map>

#include "json.hpp"
using json = nlohmann::json;

const char* ENTITY_FIFO_PATH = "/data/data/" PACKAGE_NAME "/imgui/esync";

struct Entity {
    std::map<std::string, std::vector<float>> bones;
    bool is_on_screen;
    bool is_bot;
    float health;
    bool is_knocked;
    bool is_visible;
    int team_id;
};

static std::vector<Entity> entities;
static std::mutex entities_mutex;
static std::atomic<bool> entity_thread_running{false};

static bool ensure_entity_fifo() {
    struct stat st;
    
    if (stat(ENTITY_FIFO_PATH, &st) == 0) {
        if (S_ISFIFO(st.st_mode)) {
            LOGI("Using existing FIFO at %s", ENTITY_FIFO_PATH);
            return true;
        } else unlink(ENTITY_FIFO_PATH);
    }
    
    if (mkfifo(ENTITY_FIFO_PATH, 0666) != 0) {
        LOGI("Failed to create entity FIFO: %s", strerror(errno));
        return false;
    }
    
    LOGI("Entity FIFO created at %s", ENTITY_FIFO_PATH);
    return true;
}

static void entity_reader_thread_func() {
    LOGI("Entity reader thread started");
    
    if (!ensure_entity_fifo()) {
        LOGI("Failed to create entity FIFO, thread exiting");
        return;
    }
    
    int fd = -1;
    
    while (entity_thread_running) {
        if (fd < 0) {
            fd = open(ENTITY_FIFO_PATH, O_RDONLY | O_NONBLOCK);
            
            if (fd < 0) {
                if (errno == ENXIO) { // No writer attached yet, this is normal
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                } else {
                    LOGI("Error opening entity FIFO: %s", strerror(errno));
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }
            }
            
            // LOGI("Entity FIFO opened successfully");
        }
        
        struct pollfd pfd = {0};
        pfd.fd = fd;
        pfd.events = POLLIN;
        
        int poll_result = poll(&pfd, 1, 100);
        
        if (poll_result < 0) {
            LOGI("Poll error on entity FIFO: %s", strerror(errno));
            close(fd);
            fd = -1;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        } else if (poll_result == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        char buffer[8192];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read < 0) {
            if (errno != EAGAIN) {
                LOGI("Error reading from entity FIFO: %s", strerror(errno));
                close(fd);
                fd = -1;
            }
            continue;
        } else if (bytes_read == 0) { // End of file - writer closed the FIFO
            close(fd);
            fd = -1;
            continue;
        }
        
        buffer[bytes_read] = '\0';
        
        try {
            std::string data(buffer);
            std::stringstream ss(data);
            std::string line;
            
            {
                std::lock_guard<std::mutex> lock(entities_mutex);
                entities.clear();
            }
            
            int entity_count = 0;
            while (std::getline(ss, line)) {
                if (line.empty()) continue;
                
                json entities_json = json::parse(line);
                
                if (!entities_json.is_array()) {
                    LOGI("Error: Expected JSON array of entities");
                    continue;
                }
                
                
                
for (const auto& entity_json : entities_json) {
    Entity entity;

    entity.bones = entity_json["bones"].get<std::map<std::string, std::vector<float>>>();
    entity.is_on_screen = entity_json["is_on_screen"];
    entity.is_bot = entity_json["is_bot"];
    entity.is_knocked = entity_json["is_knocked"];
    entity.is_visible = entity_json["is_visible"];

    {
        std::lock_guard<std::mutex> lock(entities_mutex);
        entities.push_back(entity);
    }

    entity_count++;
}
            }
            /*
            if (entity_count > 0) {
                // LOGI("Received %i entities", entity_count);
            }*/
        } catch (const std::exception& e) {
            LOGI("Error parsing entity JSON: %s", e.what());
        }
    }
               
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    
    LOGI("Entity reader thread stopped");
}

static void start_entity_reader() {
    if (entity_thread_running.exchange(true)) {
        LOGI("Entity reader thread already running");
        return;
    }
    
    std::thread(entity_reader_thread_func).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

static void stop_entity_reader() {
    entity_thread_running = false;
    LOGI("Stopping entity reader thread...");
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

static std::vector<Entity> get_entities() {
    std::lock_guard<std::mutex> lock(entities_mutex);
    return entities;
}

