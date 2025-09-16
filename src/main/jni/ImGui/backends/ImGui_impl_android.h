// dear ImGui: Platform Binding for Android native app
// This needs to be used along with the OpenGL 3 Renderer (ImGui_impl_opengl3)

// Implemented features:
//  [X] Platform: Keyboard arrays indexed using AKEYCODE_* codes, e.g. ImGui::IsKeyPressed(AKEYCODE_SPACE).
// Missing features:
//  [ ] Platform: Clipboard support.
//  [ ] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [ ] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'. FIXME: Check if this is even possible with Android.
// Important:
//  - FIXME: On-screen keyboard currently needs to be enabled by the application (see examples/ and issue #3446)
//  - FIXME: Unicode character inputs needs to be passed by Dear ImGui by the application (see examples/ and issue #3446)

// You can use unmodified ImGui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire ImGui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of ImGui.cpp.
// Read online: https://github.com/ocornut/ImGui/tree/master/docs

#pragma once

struct ANativeWindow;
struct AInputEvent;

ImGui_IMPL_API bool     ImGui_ImplAndroid_Init(ANativeWindow* window = 0);
ImGui_IMPL_API int32_t  ImGui_ImplAndroid_HandleInputEvent(AInputEvent* input_event, ImVec2 screen_scale);
ImGui_IMPL_API void     ImGui_ImplAndroid_Shutdown();
ImGui_IMPL_API void     ImGui_ImplAndroid_NewFrame(int screen_width = 0, int screen_height = 0);
