#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y, target_x, target_y;
    int zoom = 1;
    bool easing = false;
};