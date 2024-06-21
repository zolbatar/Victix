#include "Minimap.h"
#include "Interface.h"

extern std::unique_ptr<World> game_world;
extern std::unique_ptr<Terrain> terrain;
ImVec2 top_left;
ImVec2 bottom_right;

void RenderMinimap(cairo_t *cr, WorldPosition &state) {
    ImGuiIO &io = ImGui::GetIO();
    auto &heights = terrain->GetHeights();

    const float divider = 2;
    const float width = Terrain::F_TERRAIN_WIDTH / divider;
    const float height = Terrain::F_TERRAIN_HEIGHT * 4 / divider;

    // Background
    cairo_save(cr);
    cairo_identity_matrix(cr);
    top_left.x = io.DisplaySize.x * Interface::GetDPIScaling() - 32 - width;
    bottom_right.x = io.DisplaySize.x * Interface::GetDPIScaling() - 32;
    top_left.y = 42;
    bottom_right.y = 42 + height;
    cairo_translate(cr,
                    io.DisplaySize.x * Interface::GetDPIScaling() - 32 - width / 2,
                    24 + height);
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
        b2Vec2 _pos = obj->GetBody()->GetPosition();
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        float sz = obj->GetMinimapSize() * state.scale;
        cairo_rectangle(cr, _pos.x - sz / 2, _pos.y - sz / 2, sz, sz);
        cairo_fill(cr);
        sz = 0.1f;// * state.scale;
        switch (obj->Type()) {
            case Type::EMPLACEMENT:
                cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
                break;
        }
        cairo_rectangle(cr, _pos.x - sz / 2, _pos.y - sz / 2, sz, sz);
        cairo_stroke(cr);
    }

    // Outline
    float x1 = io.DisplaySize.x * Interface::GetDPIScaling() / 2 / state.scale - state.offset_x;
    float x2 = io.DisplaySize.x * Interface::GetDPIScaling() / state.scale;
    cairo_move_to(cr, -x1, -Terrain::F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -x1 + x2, -Terrain::F_TERRAIN_HEIGHT);
    cairo_move_to(cr, -x1, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_line_to(cr, -x1 + x2, Terrain::F_TERRAIN_HEIGHT * 3);
    cairo_set_source_rgb(cr, 0.5, 1.0, 1.0);
    cairo_set_line_width(cr, 2.0 * state.scale * divider);
    cairo_stroke(cr);
    cairo_restore(cr);

    top_left.x /= Interface::GetDPIScaling();
    top_left.y /= Interface::GetDPIScaling();
    bottom_right.x /= Interface::GetDPIScaling();
    bottom_right.y /= Interface::GetDPIScaling();
}

bool IsPointInRect(ImVec2 point, ImVec2 rectMin, ImVec2 rectMax) {
    return point.x >= rectMin.x && point.x <= rectMax.x && point.y >= rectMin.y && point.y <= rectMax.y;
}

bool MinimapCheckDrag(ImVec2 &pos, WorldPosition &state) {
    return (IsPointInRect(pos, top_left, bottom_right));
}

void MinimapCheckClick(ImVec2 &pos, WorldPosition &state) {
    if (IsPointInRect(pos, top_left, bottom_right)) {
        float fraction = (pos.x - top_left.x) / (bottom_right.x - top_left.x);
        state.offset_x = (fraction * Terrain::F_TERRAIN_WIDTH) - (Terrain::F_TERRAIN_WIDTH / 2.0);
    }
}
