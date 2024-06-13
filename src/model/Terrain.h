#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"

class Terrain {
private:
    const int TERRAIN_HEIGHT = 600;
    const int TERRAIN_WIDTH = 8192;
    const float SCALE = 0.01f;
    std::vector<float> heights;

public:
    Terrain();
    void Render(cairo_t *cr);
    void GenerateTerrain(PerlinNoise &perlin);
};
