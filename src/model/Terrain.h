#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"
#include "WorldPosition.h"
#include <box2d/box2d.h>
#include "World.h"

class Terrain {
private:
    const int DEPTH = 4;
    const float FREQ = 0.01f;
    std::vector<double> heights;
    b2Body *groundBody;
    b2Fixture *fixture = nullptr;

public:
    static const int TERRAIN_HEIGHT = 64;
    static const int TERRAIN_WIDTH = 2048;
    constexpr static const double MIN_HEIGHT = 0.75;
    constexpr static const double F_TERRAIN_HEIGHT = (double) TERRAIN_HEIGHT;
    constexpr static const double F_TERRAIN_WIDTH = (double) TERRAIN_WIDTH;

    std::vector<double> &GetHeights() { return heights; }

    Terrain();

    void Render(cairo_t *cr, WorldPosition &state);

    void GenerateTerrain(PerlinNoise &perlin);

    void UpdateBox2D();

};
