#pragma once

#include <vector>
#include "PerlinNoise.h"

class Terrain {
private:
    const int TERRAIN_HEIGHT = 600;
    const int TERRAIN_WIDTH = 800;
    const float SCALE = 0.01f;
    std::vector<float> heights;

public:
    Terrain();
    void Render();
    void GenerateTerrain(PerlinNoise &perlin);
};
