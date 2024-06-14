#include "imgui.h"
#include "World.h"

World::World() {

    // Terrain scales etc;
    ImGuiIO &io = ImGui::GetIO();
    pos.scale_x = 0.1f;
    pos.scale_y = Terrain::TERRAIN_HEIGHT / 4;
    pos.offset_x = io.DisplaySize.x / 2 - (Terrain::TERRAIN_WIDTH / 2 * pos.scale_x);
    pos.offset_y = io.DisplaySize.y / 2;
}

void World::Render(cairo_t *cr) {

    // Background
    cairo_set_source_rgb(cr, 0.05, 0.05, 0.1);
    cairo_paint(cr);

    // Terrain
    terrain.Render(cr, pos);

/*	PerlinNoise noise;
	terrain.GenerateTerrain(noise);*/
}

void World::Process() {
    ImVec2 pos = ImGui::GetMousePos();
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        printf("Dragging with left mouse button: (%.1f, %.1f)\n", drag_delta.x, drag_delta.y);
    }
}
