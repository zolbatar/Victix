#pragma once

#include <cairo.h>
#include "Terrain.h"
#include "WorldPosition.h"

class World {
private:
    Terrain terrain;
    WorldPosition state;
    ImVec2 last_drag;
    bool dragging = false;

public:
    World();
    void Render(cairo_t *cr);
    void Process();
};
