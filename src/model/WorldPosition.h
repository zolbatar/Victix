#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y;
    int zoom = 1;
    int credits = 10000;
};