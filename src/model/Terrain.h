#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"
#include "WorldPosition.h"

class Terrain {
private:
    const float SCALE = 1.0f;
    const int DEPTH = 3;
    const float FREQ = 0.01f;
    std::vector<double> heights;

public:
    static const int TERRAIN_HEIGHT = 64;
    static const int TERRAIN_WIDTH = 512;
    constexpr static const double F_TERRAIN_HEIGHT = (double) TERRAIN_HEIGHT;
    constexpr static const double F_TERRAIN_WIDTH = (double) TERRAIN_WIDTH;

    std::vector<double> &GetHeights() { return heights; }

    Terrain();

    void Render(cairo_t *cr, WorldPosition &pos);

    void GenerateTerrain(PerlinNoise &perlin);
};
