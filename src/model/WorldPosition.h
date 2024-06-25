#pragma once

#include "imgui.h"

class WorldPosition {
public:
    float scale, offset_x, offset_y;
    static float shoot_delta_x;
    static float shoot_delta_y;
    static int credits;
    static int cost;
};
