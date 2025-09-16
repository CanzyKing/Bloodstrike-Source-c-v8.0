#pragma once

#include "ImGui.h"
#include "ImGui_internal.h"

namespace ImGui {
	ImGui_API void LoadStyleFrom(const char* fileName);
	ImGui_API void SaveStylesTo(const char* fileName);
}
