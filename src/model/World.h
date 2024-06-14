#pragma once

#include <GLFW/glfw3.h>
#include <cairo.h>
#include "Terrain.h"
#include "WorldPosition.h"

class World {
private:
    Terrain terrain;
    WorldPosition state;
    ImVec2 last_drag;
    bool dragging = false;
    void DoZoom(int vzoom);

public:
    World();
    void Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height);
    void Process();
};
