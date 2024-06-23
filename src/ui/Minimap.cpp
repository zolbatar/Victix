#include <include/effects/SkImageFilters.h>
#include "Minimap.h"
#include "Interface.h"

extern std::unique_ptr<World> game_world;
extern std::unique_ptr<Terrain> terrain;
ImVec2 top_left;
ImVec2 bottom_right;

void RenderMinimap(WorldPosition &state) {
    ImGuiIO &io = ImGui::GetIO();
    auto &heights = terrain->GetHeights();
    auto canvas = Skia::GetCanvas();

    const float divider = 2;
    const float width = Terrain::F_TERRAIN_WIDTH / divider;
    const float height = Terrain::F_TERRAIN_HEIGHT * 4 / divider;

    // 752
    // 1604

    // Background
    canvas->save();
    canvas->translate(io.DisplaySize.x * Interface::GetDPIScaling() - 32 - Terrain::F_TERRAIN_WIDTH / 2,
                      24);
    canvas->scale(1.0f / divider, 1.0f / divider);
    top_left = Skia::Reverse(0, 0);
    bottom_right = Skia::Reverse(width, height);

    // backgrounds
    auto rect = SkRect::MakeXYWH(0, 0, Terrain::F_TERRAIN_WIDTH, Terrain::F_TERRAIN_HEIGHT * 4);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setARGB(64, 0, 0, 0);
    canvas->drawRect(rect, paint);

    // Build points
    SkPoint _points[Terrain::TERRAIN_WIDTH];
    for (unsigned int i = 0; i < Terrain::TERRAIN_WIDTH; i++) {
        _points[i] = SkPoint::Make((float) i, (float) heights[i]);
    }

    // Draw path
    SkPath path;
    path.moveTo(_points[0].x(), _points[0].y() + Terrain::F_TERRAIN_HEIGHT * 2);
    for (int i = 1; i < sizeof(_points) / sizeof(_points[0]); ++i) {
        path.lineTo(_points[i].x(), _points[i].y() + Terrain::F_TERRAIN_HEIGHT * 2);
    }
    SkPath outer;
    outer = path;
    path.lineTo(Terrain::F_TERRAIN_WIDTH, Terrain::F_TERRAIN_HEIGHT * 4);
    path.lineTo(0, Terrain::F_TERRAIN_HEIGHT * 4);

    // Fill
    paint.reset();
    paint.setColor(SkColorSetARGB(255, 25, 25, 112));
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    canvas->drawPath(path, paint);

    // Outline
    paint.reset();
    paint.setARGB(255, 255, 255, 255); // Outline color
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setAntiAlias(true);
    paint.setStrokeWidth(2.0f / state.scale * divider); // Set the stroke width
    canvas->drawPath(outer, paint);

    // And objects
/*    for (auto &obj: game_world->GetObjects()) {
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
    }*/

    // Outline
    SkPath outline;
    float x1 = state.offset_x;
    float x2 = io.DisplaySize.x * Interface::GetDPIScaling() / state.scale;
    x1 += x2;
    outline.moveTo(x1, 10);
    outline.lineTo(x1 + x2, 10);
    outline.moveTo(x1, Terrain::F_TERRAIN_HEIGHT * 4);
    outline.lineTo(x1 + x2, Terrain::F_TERRAIN_HEIGHT * 4);
    paint.reset();
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setARGB(255, 255, 255, 255);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(1.0f * state.scale * divider);
//    outline.close();
    canvas->drawPath(outline, paint);

/*    top_left.x /= Interface::GetDPIScaling();
    top_left.y /= Interface::GetDPIScaling();
    bottom_right.x /= Interface::GetDPIScaling();
    bottom_right.y /= Interface::GetDPIScaling();*/
    canvas->restore();
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
