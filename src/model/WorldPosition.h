#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y;
    int zoom = 0;
    const float zooms[20] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 12.0, 14.0, 16.0, 20.0, 24.0,
                             28.0, 32.0, 36.0};
    const int count_zooms = sizeof(zooms) / sizeof(float);
};