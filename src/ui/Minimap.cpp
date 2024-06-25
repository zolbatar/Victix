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

    // Background
    const float x_divider = 2.0f;
    const float y_divider = 2.0f;
    canvas->save();
    canvas->scale(1.0f / x_divider, 1.0f / y_divider);
    canvas->translate(io.DisplaySize.x * Interface::GetDPIScaling() - Terrain::F_TERRAIN_WIDTH / 2, 32);
    top_left.x = (io.DisplaySize.x / 2) - Terrain::F_TERRAIN_WIDTH / 2 / x_divider / Interface::GetDPIScaling();
    bottom_right.x = (io.DisplaySize.x / 2) + Terrain::F_TERRAIN_WIDTH / 2 / x_divider / Interface::GetDPIScaling();
    top_left.y = 12;
    bottom_right.y = 74;

    // backgrounds
    auto rect = SkRect::MakeXYWH(0, 0, Terrain::F_TERRAIN_WIDTH, Terrain::F_TERRAIN_HEIGHT * 6);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setARGB(64, 0, 0, 0);
    canvas->drawRect(rect, paint);

    // Build points
    float adj = Terrain::F_TERRAIN_HEIGHT * 5;
    SkPoint _points[Terrain::TERRAIN_WIDTH];
    for (unsigned int i = 0; i < Terrain::TERRAIN_WIDTH; i++) {
        _points[i] = SkPoint::Make((float) i, adj - (float) heights[i]);
    }

    // Draw path
    SkPath path;
    path.moveTo(_points[0].x(), _points[0].y());
    for (int i = 1; i < sizeof(_points) / sizeof(_points[0]); ++i) {
        path.lineTo(_points[i].x(), _points[i].y());
    }
    SkPath outer;
    outer = path;
    path.lineTo(Terrain::F_TERRAIN_WIDTH, Terrain::F_TERRAIN_HEIGHT * 6);
    path.lineTo(0, Terrain::F_TERRAIN_HEIGHT * 6);

    // Fill
    paint.reset();
    paint.setColor(SkColorSetARGB(64, 25, 25, 112));
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    canvas->drawPath(path, paint);

    // Outline
    paint.reset();
    paint.setARGB(64, 255, 255, 255); // Outline color
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setAntiAlias(true);
    paint.setStrokeWidth(1.0f * x_divider); // Set the stroke width
    canvas->drawPath(outer, paint);

    // And objects
    for (auto &obj: game_world->GetObjects()) {
        b2Vec2 _pos = obj->GetBody()->GetPosition();
        float sz = obj->GetMinimapSize() * x_divider;
        rect = SkRect::MakeXYWH(_pos.x + (Terrain::TERRAIN_WIDTH / 2) - sz / 2,
                                (Terrain::TERRAIN_HEIGHT * 5) - _pos.y - sz / 2,
                                sz, sz);
        paint.setStyle(SkPaint::Style::kFill_Style);
        switch (obj->GetPlayer()) {
            case Player::ENEMY:
                paint.setARGB(128, 255, 0, 0);
                break;
            case Player::FRIENDLY:
                paint.setARGB(128, 0, 0, 255);
                break;
        }
        canvas->drawRect(rect, paint);

        paint.setARGB(128, 255, 255, 0);
        paint.setStrokeWidth(1.0f * x_divider); // Set the stroke width
        paint.setStyle(SkPaint::Style::kStroke_Style);
        canvas->drawRect(rect, paint);
    }

    // Outline
    SkPath outline;
    float x1 = state.offset_x;
    float x2 = io.DisplaySize.x * Interface::GetDPIScaling() / state.scale;
    x1 += x2;
    outline.moveTo(x1, 10);
    outline.lineTo(x1 + x2, 10);
    outline.moveTo(x1, Terrain::F_TERRAIN_HEIGHT * 6);
    outline.lineTo(x1 + x2, Terrain::F_TERRAIN_HEIGHT * 6);
    paint.reset();
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setARGB(255, 255, 255, 255);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(1.0f * state.scale * x_divider);
//    outline.close();
    canvas->drawPath(outline, paint);

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
