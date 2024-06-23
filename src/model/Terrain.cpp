#include <random>
#include <include/core/SkPoint.h>
#include <include/core/SkPath.h>
#include <include/effects/SkImageFilters.h>
#include "Terrain.h"
#include "../ui/Interface.h"
#include "World.h"
#include "../objects/Emplacement.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPoint3.h"

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
    original_heights = heights;

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

void Terrain::RenderSkia(WorldPosition &state, int alpha, bool flip_x, bool flip_y, SkColor colour) {
    ImGuiIO &io = ImGui::GetIO();
    auto canvas = Skia::GetCanvas();

    // Build points
    SkPoint _points[Terrain::TERRAIN_WIDTH];
    for (unsigned int i = 0; i < Terrain::TERRAIN_WIDTH; i++) {
        if (flip_x || flip_y)
            _points[i] = SkPoint::Make(i - F_TERRAIN_WIDTH / 2, original_heights[i]);
        else
            _points[i] = SkPoint::Make(i - F_TERRAIN_WIDTH / 2, heights[i]);
    }

    // Draw path
    SkPath path;
    path.moveTo(_points[0].x(), _points[0].y());
    for (int i = 1; i < sizeof(_points) / sizeof(_points[0]); ++i) {
        path.lineTo(_points[i].x(), _points[i].y());
    }
    SkPath outer(path);
    path.lineTo((F_TERRAIN_WIDTH / 2), F_TERRAIN_HEIGHT * 2);
    path.lineTo(-(F_TERRAIN_WIDTH / 2), F_TERRAIN_HEIGHT * 2);
    path.close();

    // Fill
    SkPaint paint;
    float adj = alpha / 255.0;
    sk_sp<SkImageFilter> dropShadow = SkImageFilters::DropShadow(
            5.0f * adj, 5.0f * adj,  // dx, dy
            10.0f * adj, 10.0f * adj,    // sigmaX, sigmaY
            SkColorSetARGB(160 * adj, 0, 0, 0), // shadow color
            nullptr        // input (nullptr means apply to the paint directly)
    );
    paint.setImageFilter(dropShadow);
    paint.setColor(colour);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    canvas->drawPath(path, paint);

    // Outline
    paint.reset();
    paint.setARGB(alpha, 255, 255, 255); // Outline color
    paint.setStyle(SkPaint::kStroke_Style); // Use stroke style
    paint.setAntiAlias(true);
    paint.setStrokeWidth(2.0f / state.scale); // Set the stroke width
    canvas->drawPath(outer, paint);
}
