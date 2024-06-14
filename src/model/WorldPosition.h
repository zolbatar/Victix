#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y;
    int zoom = 0;
    const float zooms[6] = {0.05f, 0.15f, 0.4f, 0.7f, 1.1f, 2.0f};
    const int count_zooms = sizeof(zooms) / sizeof(float);

    double ConvertWorldToScreenX(double x) {
        ImGuiIO &io = ImGui::GetIO();
        return (x * scale) - (offset_x * scale) + (io.DisplaySize.x / 2);
    }

    double ConvertWorldToScreenY(double y) {
        ImGuiIO &io = ImGui::GetIO();
        return (io.DisplaySize.y / 2) - (y * scale) + (offset_y * scale);
    }
};