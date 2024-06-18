#include "Minimap.h"

extern std::unique_ptr<World> game_world;

void RenderMinimap(cairo_t *cr) {
    ImGuiIO &io = ImGui::GetIO();

    const float divider = 8;
    const float width = Terrain::F_TERRAIN_WIDTH * 2/ divider;
    const float height = Terrain::F_TERRAIN_HEIGHT * 4 / divider;

    // Backgroud & outline
    cairo_identity_matrix(cr);
    cairo_rectangle(cr, io.DisplaySize.x - width - 32, 32, width, height);
    cairo_set_source_rgba(cr, 0, 0, 0, 32.0 / 255.0);
    cairo_fill(cr);
    cairo_rectangle(cr, io.DisplaySize.x - width - 32, 32, width, height);
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);
}
