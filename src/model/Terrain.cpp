#include <iostream>
#include "Terrain.h"
#include "imgui.h"

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double height = perlin.noise(x * SCALE, 0.0, 0.1, 5);
        heights[x] = height;
    }
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::Render(cairo_t *cr) {
    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImGuiIO &io = ImGui::GetIO();
    /*double screen_width = io.DisplaySize.x;
    double screen_height = io.DisplaySize.y;*/

    // Terrain scales etc;
    float scale_x = 0.1f;
    float scale_y = TERRAIN_HEIGHT / 4;
    float offset_x = io.DisplaySize.x / 2 - (TERRAIN_WIDTH / 2 * scale_x);
    float offset_y = TERRAIN_HEIGHT / 2;

    // Calculate rectangle coordinates
    ImVec2 rectMin = ImVec2(io.DisplaySize.x / 2 - (TERRAIN_WIDTH / 2 * scale_x), io.DisplaySize.y / 2 - (TERRAIN_HEIGHT / 2 * scale_y));
    ImVec2 rectMax = ImVec2(io.DisplaySize.x / 2 + (TERRAIN_WIDTH / 2 * scale_x), io.DisplaySize.y / 2 + (TERRAIN_HEIGHT / 2 * scale_y));

    // Define gradient colors for each corner
    ImU32 col_tl = IM_COL32(255, 0, 0, 255);   // Top-left: Red
    ImU32 col_tr = IM_COL32(0, 255, 0, 255);   // Top-right: Green
    ImU32 col_br = IM_COL32(0, 0, 255, 255);   // Bottom-right: Blue
    ImU32 col_bl = IM_COL32(255, 255, 0, 255); // Bottom-left: Yellow

    // Draw gradient-filled rectangle
    //dl->AddRectFilledMultiColor(rectMin, rectMax, col_tl, col_tr, col_br, col_bl);

    cairo_set_source_rgba(cr, 255, 255, 255, 255);
//    cairo_set_line_width(cr, 2.5);
    ImVec2 triangle[TERRAIN_WIDTH];
    for (int i = 0; i < TERRAIN_WIDTH; i++) {
        triangle[i].x = i * scale_x + offset_x;
        triangle[i].y = heights[i] * scale_y + offset_y;
/*        ImVec2 p1{i * scale_x + offset_x, heights[i] * scale_y + offset_y};
        ImVec2 p2{(i + 1) * scale_x + offset_x, heights[i + 1] * scale_y + offset_y};
        cairo_move_to(cr, i * scale_x + offset_x, heights[i] * scale_y + offset_y);
        cairo_line_to(cr, (i + 1) * scale_x + offset_x, heights[i + 1] * scale_y + offset_y);
        cairo_stroke(cr);*/
        //dl->AddLine(p1, p2, IM_COL32(128, 128, 128, 255));
    }
    dl->AddPolyline(triangle, TERRAIN_WIDTH, IM_COL32(255, 255, 255, 255), ImDrawFlags_Closed, 1.0f);
}
