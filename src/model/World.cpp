#include "imgui.h"
#include "World.h"

World::World() {

    // Terrain scales etc;
    ImGuiIO &io = ImGui::GetIO();
    state.scale_x = 0.1f;
    state.scale_y = Terrain::TERRAIN_HEIGHT / 4;
    state.offset_x = io.DisplaySize.x / 2 - (Terrain::TERRAIN_WIDTH / 2 * state.scale_x);
    state.offset_y = io.DisplaySize.y / 2;
}

void World::Render(cairo_t *cr) {

    // Background
    cairo_set_source_rgb(cr, 0.05, 0.05, 0.1);
    cairo_paint(cr);

    // Terrain
    terrain.Render(cr, state);

/*	PerlinNoise noise;
	terrain.GenerateTerrain(noise);*/
}

void World::Process() {
    ImVec2 pos = ImGui::GetMousePos();
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        if (!dragging) {
            dragging = true;
            last_drag = drag_delta;
        } else {
            float drag_scale = 1.0;
            state.offset_x += (drag_delta.x - last_drag.x) * drag_scale;
            state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
            printf("Dragging with left mouse button: (%.1f, %.1f)\n", drag_delta.x, drag_delta.y);
            last_drag = drag_delta;
        }
    } else {
        dragging = false;
    }
}
