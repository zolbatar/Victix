#include "imgui.h"
#include "World.h"

World::World() {
    ImGuiIO &io = ImGui::GetIO();
    state.zoom = 0;
    state.scale = state.zooms[state.zoom];
    state.offset_x = 0.0f;
    state.offset_y = 0.0f;
}

void World::Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height) {

    // Background
    cairo_set_source_rgb(cr, 0.05, 0.05, 0.1);
    cairo_paint(cr);

    // Terrain
    terrain.Render(cr, state);

    // Write to texture and blit
    cairo_surface_flush(surface);
    glBindTexture(GL_TEXTURE_2D, render);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 cairo_image_surface_get_data(surface));
    ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(render),
            ImVec2(0.0f, 0.0f),
            ImVec2(width, height),
            ImVec2(0.0f, 0.0f),
            ImVec2(1.0f, 1.0f),
            IM_COL32(255, 255, 255, 255));

    ImGui::BeginChild("Position", ImVec2(200, 200), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::Text("Position: %.2f %.2f", state.offset_x, state.offset_y);
    ImGui::Text("Scale: %.2f %d", state.scale, state.zoom);
    ImGui::PopStyleColor();
    ImGui::EndChild();

/*	PerlinNoise noise;
	terrain.GenerateTerrain(noise);*/
}

void World::DoZoom(int vzoom) {
    state.zoom += vzoom;
    if (state.zoom < 0)
        state.zoom = 0;
    if (state.zoom >= state.count_zooms)
        state.zoom = state.count_zooms - 1;
    state.scale = state.zooms[state.zoom];
}

void World::Process() {
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();

    // Zoom
    if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        DoZoom(1);
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, false)) {
        DoZoom(-1);
    } else if (io.MouseWheel != 0.0f) {
        if (io.MouseWheel < 0)
            DoZoom(-1);
        else
            DoZoom(1);
    }

    // Dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        if (!dragging) {
            dragging = true;
            last_drag = drag_delta;
        } else {
            float drag_scale = 5.0 / (1.0 + state.scale);
            state.offset_x -= (drag_delta.x - last_drag.x) * drag_scale;
            state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
            last_drag = drag_delta;
        }
    } else {
        dragging = false;
    }
}
