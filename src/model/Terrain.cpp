#include <iostream>
#include "Terrain.h"
#include "imgui.h"

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double height = perlin.noise(x * SCALE, 0.0, 1.0, 5);
        heights[x] = height;
    }
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::Render() {
    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImGuiIO &io = ImGui::GetIO();
    /*double screen_width = io.DisplaySize.x;
    double screen_height = io.DisplaySize.y;*/

    float scale_x = 1.0f;
    float scale_y = TERRAIN_HEIGHT / 2;
    float offset_x = 0.0f;
    float offset_y = TERRAIN_HEIGHT / 2;
    for (int i = 0; i < TERRAIN_WIDTH - 1; i++) {
        ImVec2 p1{i * scale_x + offset_x, heights[i] * scale_y + offset_y};
        ImVec2 p2{(i + 1) * scale_x + offset_x, heights[i + 1] * scale_y + offset_y};
        dl->AddLine(p1, p2, IM_COL32(128, 128, 128, 255));
    }
}
