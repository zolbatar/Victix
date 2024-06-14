#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"
#include "WorldPosition.h"

class Terrain {
private:
    const float SCALE = 1.5f;
    const float FREQ = 0.001f;
    const int DEPTH = 5;
    std::vector<float> heights;

public:
    static const int TERRAIN_HEIGHT = 2048;
    static const int TERRAIN_WIDTH = 16384;

    Terrain();
    void Render(cairo_t *cr, WorldPosition &pos);
    void GenerateTerrain(PerlinNoise &perlin);
};
