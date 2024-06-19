#include "Minimap.h"
#include "Interface.h"

extern std::unique_ptr<World> game_world;
ImVec2 top_left;
ImVec2 bottom_right;

void RenderMinimap(cairo_t *cr, std::vector<double> &heights, WorldPosition &state) {
    ImGuiIO &io = ImGui::GetIO();

    const float divider = 4;
    const float width = Terrain::F_TERRAIN_WIDTH / divider;
    const float height = Terrain::F_TERRAIN_HEIGHT * 4 / divider;

    // Background
    cairo_save(cr);
    cairo_identity_matrix(cr);
    top_left.x = io.DisplaySize.x - 32 - width;
    bottom_right.x = io.DisplaySize.x - 32;
    top_left.y = 24;
    bottom_right.y = 24 + height;
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
        b2Vec2 _pos = obj->GetBody()->GetPosition();
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_rectangle(cr, _pos.x, _pos.y, sz, sz);
        cairo_fill(cr);
    }

    // Outline
    float x1 = io.DisplaySize.x / 2 / state.scale - state.offset_x;
    float x2 = io.DisplaySize.x / state.scale;
    cairo_move_to(cr, -x1, -Terrain::F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -x1 + x2, -Terrain::F_TERRAIN_HEIGHT);
    cairo_move_to(cr, -x1, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_line_to(cr, -x1 + x2, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, divider * 2);
    cairo_stroke(cr);
    cairo_restore(cr);
}

bool IsPointInRect(ImVec2 point, ImVec2 rectMin, ImVec2 rectMax) {
    return point.x >= rectMin.x && point.x <= rectMax.x && point.y >= rectMin.y && point.y <= rectMax.y;
}

void MinimapCheckClick(ImVec2 &pos, WorldPosition &state) {
    if (IsPointInRect(pos, top_left, bottom_right)) {
        float fraction = (pos.x - top_left.x) / (bottom_right.x - top_left.x);
        state.offset_x = (fraction * Terrain::F_TERRAIN_WIDTH) - (Terrain::F_TERRAIN_WIDTH / 2.0);
    }
}
