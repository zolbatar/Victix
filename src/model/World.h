#pragma once

#include <list>
#include <memory>
#include <GLFW/glfw3.h>
#include <cairo.h>
#include "Terrain.h"
#include "WorldPosition.h"
#include "../ui/CairoDebugDraw.h"
#include <box2d/box2d.h>
#include "../objects/Object.h"

class World {
private:
    WorldPosition state;
    ImVec2 last_drag;
    bool dragging = false;

    // Smooth scroll animation
    double startTime = glfwGetTime();
    double duration = 0.25; // Animation duration in seconds
    double startValue = 0.0; // Start value
    double endValue = 100.0; // End value

    // Box2D
    const float timeStep = 1.0f / 60.0f;
    const int32 velocityIterations = 6;
    const int32 positionIterations = 2;
    b2Body *groundBody;
    CairoDebugDraw cairoDebugDraw;
    std::list<std::unique_ptr<Object>> objects;

    void DoZoom(int vzoom);

public:
    World();

    void Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height);

    void Process();

    std::list<std::unique_ptr<Object>> &GetObjects() { return objects; }
};
