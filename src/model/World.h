#pragma once

#include <list>
#include <memory>
#include <GLFW/glfw3.h>
#include <cairo.h>
#include "Terrain.h"
#include "WorldPosition.h"
#include "../ui/CairoDebugDraw.h"
#include <box2d/box2d.h>
#include "Object.h"

class World {
private:
    Terrain terrain;
    WorldPosition state;
    ImVec2 last_drag;
    bool dragging = false;

    // Box2D
    const float timeStep = 1.0f / 60.0f;
    const int32 velocityIterations = 6;
    const int32 positionIterations = 2;
    std::shared_ptr<b2World> world;
    b2Body *groundBody;
    CairoDebugDraw cairoDebugDraw;
    std::list<Object> objects;

    void DoZoom(int vzoom);

public:
    World();

    void Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height);

    void Process();
};
