#pragma once

#include <vector>
#include "PerlinNoise.h"
#include "WorldPosition.h"
#include <box2d/box2d.h>
#include "World.h"
#include "../ui/Skia.h"
#include <include/core/SkPaint.h>

class Terrain {
private:
    const int DEPTH = 3;
    const float FREQ = 0.01f;
    std::vector<double> heights;
    std::vector<double> original_heights;
    b2Body *groundBody;
    b2Fixture *fixture = nullptr;

public:
    static const int TERRAIN_HEIGHT = 64;
    static const int TERRAIN_WIDTH = 2048;
    constexpr static const double F_TERRAIN_HEIGHT = (double) TERRAIN_HEIGHT;
    constexpr static const double F_TERRAIN_WIDTH = (double) TERRAIN_WIDTH;

    std::vector<double> &GetHeights() { return heights; }

    std::vector<double> &GetOriginalHeights() { return original_heights; }

    Terrain();

    void RenderSkia(WorldPosition &state, int alpha, bool flip_x, bool flip_y, SkColor colour);

    void GenerateTerrain(PerlinNoise &perlin);

    void UpdateBox2D();

};
