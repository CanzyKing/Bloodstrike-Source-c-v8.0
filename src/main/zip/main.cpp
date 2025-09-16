#define PACKAGE_NAME "com.netease.newspike"
#include "include/obfuscate.h"
#include "include/includes.h"
#include "And64InlineHook/And64InlineHook.hpp"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/MemoryPatch.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/KittyUtils.h"
#include "include/hpp/srcpy.h"
#include "Dobby/dobby.h"
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include "block.h"
#include "expired.h"

DEFINES(void*, _v07, const char *filename, int flags) { return nullptr; }
DEFINES(void*, _xv07, const char *filename, int flags) { return nullptr; }

static int iLoaded = 0, iLoadAt = 2;
DEFINES(void*, iMissMyWife, const char* v0, int v1, void* v2, void* v3, void* v4) {
    static bool loaded = false;
    if (v1 == 257) {
        iLoaded++;
        if (iLoaded >= iLoadAt && !loaded) {
            loaded = true;
            _iMissMyWife(srcpy, 257, v2, v2, NULL);
        }
    }
    return _iMissMyWife(v0, v1, v2, v3, v4);
}

#include "draw.h"
#include "include/input.h"

void *__1__(void *) {
    install_domain_blocker();
    
    if (ExpiredSystem::IsExpired()) {
        LOGI("Mod expired. Notification system activated.");
        
    }
 
    sleep(3);

    if (!fdlopen("libGame.so", 0)) return 0;

    auto libGame = absoluteAddress("libGame.so");
    if (!libGame) return 0;

    create_directory_recursive("/data/data/" PACKAGE_NAME "/imgui");

    HOOKI((ptr) libGame + 0x44F5334, iMissMyWife);
    var.Feature1 = MemoryPatch::createWithHex(libGame + 0x16B6444, "0000A0C0");
    
    __IMGUI__(); 
    __INPUT__();

    LOGI("AKU NAKKK EWWWW FURINAAA SGSSSS");
    return 0;
}

__attribute__((constructor))
void __0__() { pthread(__1__); }