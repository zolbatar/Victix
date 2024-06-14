#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float reset_scale_x, reset_scale_y;
    float scale_x, scale_y, offset_x, offset_y;
    const float zoom_adjust = 0.01f;
    const float max_scale_x = 1.0f;

    double ConvertWorldToScreenX(double x) {
        ImGuiIO &io = ImGui::GetIO();
        return (x * scale_x) - (offset_x * scale_x) + (io.DisplaySize.x / 2);
    }

    double ConvertWorldToScreenY(double y) {
        ImGuiIO &io = ImGui::GetIO();
        return (io.DisplaySize.y / 2) - (y * scale_y) + (offset_y * scale_y);
    }

    double ConvertScreenToWorldX(double x) {
        ImGuiIO &io = ImGui::GetIO();
        return ((x / scale_x) - (io.DisplaySize.x / 2) + offset_x) / scale_x;
    }

    double ConvertScreenToWorldY(double y) {
        ImGuiIO &io = ImGui::GetIO();
        return ((io.DisplaySize.y / 2) - (y / scale_y) + offset_y) / scale_y;
    }

};