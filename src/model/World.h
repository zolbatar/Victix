#pragma once

#include <cairo.h>
#include "Terrain.h"

class World {
private:
    Terrain terrain;

public:
    void Render(cairo_t *cr);
};
