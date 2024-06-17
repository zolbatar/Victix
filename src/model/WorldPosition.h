#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y;
    int zoom = 0;
//    const float zooms[6] = {0.05f, 0.15f, 0.4f, 0.7f, 1.1f, 2.0f};
    const float zooms[10] = {1.0, 2.0, 3.0, 4.0, 6.0, 8.0, 12.0, 16.0, 24.0, 32.0};
//    const float zooms[14] = {0.051, 0.065, 0.086, 0.114, 0.151, 0.200, 0.263, 0.345, 0.454, 0.598, 0.788, 1.036, 1.361, 2.0};
    const int count_zooms = sizeof(zooms) / sizeof(float);
};