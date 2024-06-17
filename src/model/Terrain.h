#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"
#include "WorldPosition.h"

class Terrain {
private:
    const float SCALE = 1.0f;
    const float FREQ = 0.01f;
    const int DEPTH = 3;
    std::vector<double> heights;

public:
    static const int TERRAIN_HEIGHT = 64;
    static const int TERRAIN_WIDTH = 256;

    std::vector<double> &GetHeights() { return heights; }

    Terrain();
    void Render(cairo_t *cr, WorldPosition &pos);
    void GenerateTerrain(PerlinNoise &perlin);
};
