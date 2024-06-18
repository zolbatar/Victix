#include "Minimap.h"
#include "Interface.h"

extern std::unique_ptr<World> game_world;

void RenderMinimap(cairo_t *cr, std::vector<double> &heights, WorldPosition &pos) {
    ImGuiIO &io = ImGui::GetIO();

    const float divider = 4;
    const float width = Terrain::F_TERRAIN_WIDTH / divider;
    const float height = Terrain::F_TERRAIN_HEIGHT * 4 / divider;

    // Background
    cairo_save(cr);
    cairo_identity_matrix(cr);
    cairo_translate(cr, io.DisplaySize.x - 32 - width / 2, 24 + height);
    cairo_scale(cr, 1.0 / divider, -1.0 / divider);
    cairo_rectangle(cr,
                    -Terrain::F_TERRAIN_WIDTH / 2, -Terrain::F_TERRAIN_HEIGHT * 1,
                    Terrain::F_TERRAIN_WIDTH, Terrain::F_TERRAIN_HEIGHT * 4);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 64.0 / 255.0);
    cairo_fill(cr);

    // Do it
    for (unsigned int i = 0; i < Terrain::TERRAIN_WIDTH; i += divider) {
        if (i == 0)
            cairo_move_to(cr, i - Terrain::F_TERRAIN_WIDTH / 2, heights[i]);
        else
            cairo_line_to(cr, i - Terrain::F_TERRAIN_WIDTH / 2, heights[i]);
    }
    cairo_line_to(cr, (Terrain::F_TERRAIN_WIDTH / 2), -Terrain::F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -(Terrain::F_TERRAIN_WIDTH / 2), -Terrain::F_TERRAIN_HEIGHT);
    cairo_close_path(cr);

    // Fill
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    cairo_fill(cr);

    // And objects
    for (auto &obj: game_world->GetObjects()) {
        float sz = obj->GetMinimapSize();
        b2Vec2 pos = obj->GetBody()->GetPosition();
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_rectangle(cr, pos.x, pos.y, sz, sz);
        cairo_fill(cr);
    }

    // Outline
    cairo_move_to(cr, -io.DisplaySize.x / 2 + pos.offset_x, -Terrain::F_TERRAIN_HEIGHT);
    cairo_line_to(cr, io.DisplaySize.x / 2 + pos.offset_x, -Terrain::F_TERRAIN_HEIGHT);
    cairo_move_to(cr, -io.DisplaySize.x / 2, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_line_to(cr, io.DisplaySize.x / 2, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, divider * 2);
    cairo_stroke(cr);
    cairo_restore(cr);
}
