#pragma once

#include <cairo.h>
#include "Terrain.h"
#include "WorldPosition.h"

class World {
private:
    Terrain terrain;
    WorldPosition pos;

public:
    World();
    void Render(cairo_t *cr);
    void Process();
};
