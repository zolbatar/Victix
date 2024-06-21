#include <random>
#include <include/core/SkPoint.h>
#include <include/core/SkPath.h>
#include <include/core/SkPaint.h>
#include "Terrain.h"
#include "../ui/Interface.h"
#include "../ui/Skia.h"
#include "World.h"
#include "../objects/Emplacement.h"

extern std::unique_ptr<b2World> world;
extern std::unique_ptr<World> game_world;

unsigned int next_power_of_2(float x);

static std::random_device rd;  // Random device to seed the generator
static std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
static std::uniform_real_distribution<> dis(0.0, Terrain::F_TERRAIN_WIDTH); // Uniform distribution in the range [0, 1)

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    double y = dis(gen);
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double pn = perlin.noise(x, y, FREQ, DEPTH) * 2.0;
        heights[x] = (pn * TERRAIN_HEIGHT) + TERRAIN_HEIGHT / 4;
//        assert(heights[x] > 0);
    }

    UpdateBox2D();
}

void Terrain::UpdateBox2D() {
    if (groundBody != nullptr) {
        world->DestroyBody(groundBody);
//        groundBody->DestroyFixture(fixture);
    }

    // Ground
    b2BodyDef groundBodyDef;
    groundBodyDef.type = b2_staticBody;
    groundBodyDef.position.Set(0.0f, 0.0f);
    groundBody = world->CreateBody(&groundBodyDef);

    // Ground shape
    int hsize = (int) heights.size();
    float hsize_half = heights.size() / 2;
    b2Vec2 vertices[hsize + 2];
    for (int i = 0; i < heights.size(); i++) {
        float x = i - hsize_half;
        vertices[hsize - i - 1].Set(x, heights[i]);
    }
    vertices[heights.size()].Set(-hsize_half, -Terrain::TERRAIN_HEIGHT);
    vertices[heights.size() + 1].Set(hsize_half, -Terrain::TERRAIN_HEIGHT);
    b2ChainShape groundBox;
    groundBox.CreateLoop(vertices, hsize + 2);
    groundBody->CreateFixture(&groundBox, 0.0f);
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::RenderSkia(WorldPosition &state) {
    ImGuiIO &io = ImGui::GetIO();
    auto canvas = Skia::GetCanvas();

    // Build points
    SkPoint _points[Terrain::TERRAIN_WIDTH];
    for (unsigned int i = 0; i < Terrain::TERRAIN_WIDTH; i++) {
        _points[i] = SkPoint::Make(i - F_TERRAIN_WIDTH / 2, heights[i]);
    }

    // Draw path
    SkPath path;
    path.moveTo(_points[0].x(), _points[0].y());
    for (int i = 1; i < sizeof(_points) / sizeof(_points[0]); ++i) {
        path.lineTo(_points[i].x(), _points[i].y());
    }
    SkPath outer(path);
    path.lineTo((F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    path.lineTo(-(F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    path.close();

    // Fill
    SkPaint paint;
    paint.setColor(SkColorSetARGB(255, 25, 25, 112));
    paint.setStrokeWidth(1.0f);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    canvas->drawPath(path, paint);

    // Outline blue
    paint.reset();
    paint.setColor(SkColorSetARGB(255, 255, 255, 255)); // Outline color
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 6.0f / state.scale));
    paint.setStrokeWidth(2.0f / state.scale); // Set the stroke width
    canvas->drawPath(outer, paint);

    // Outline
    paint.reset();
    paint.setColor(SkColorSetARGB(128, 255, 255, 255)); // Outline color
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setAntiAlias(true);
    paint.setStrokeWidth(2.0f / state.scale); // Set the stroke width
    canvas->drawPath(outer, paint);
}

void Terrain::Render(cairo_t *cr, WorldPosition &state) {
    if (game_world->add_mode) {
        ImGuiIO &io = ImGui::GetIO();
        ImVec2 mouse_position = ImGui::GetMousePos();
        float x = (mouse_position.x - io.DisplaySize.x / 2) * Interface::GetDPIScaling() / state.scale + state.offset_x;
        float y = (io.DisplaySize.y - mouse_position.y) * Interface::GetDPIScaling() / state.scale + state.offset_y;
        Emplacement::AddEmplacement(cr, x, y, false, Player::FRIENDLY);
    }

    // Do it
    for (unsigned int i = 0; i < TERRAIN_WIDTH; i++) {
        if (i == 0)
            cairo_move_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
        else
            cairo_line_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, (F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -(F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    cairo_close_path(cr);

    // Fill
    cairo_set_source_rgb(cr, 25.0 / 255.0, 25.0 / 255.0, 112.0 / 255.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgb(cr, 0.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);

    if (game_world->add_mode) {
        Emplacement::Restore();
    }
}
