#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"

class Terrain {
private:
    const int TERRAIN_HEIGHT = 500;
    const int TERRAIN_WIDTH = 8192;
    const float SCALE = 2.5f;
    const float FREQ = 0.0005f;
    const int DEPTH = 3;
    std::vector<float> heights;

    const double layer1[3] = {24.41 / 100.0, 13.04 / 100.0, 7.55 / 100.0};
    const double layer2[3] = {33.74 / 100.0, 26.67 / 100.0, 21.18 / 100.0};
    const double layer3[3] = {71.8 / 100.0, 69.45 / 100.0, 66.72 / 100.0};

public:
    Terrain();
    void Render(cairo_t *cr);
    void GenerateTerrain(PerlinNoise &perlin);
};
