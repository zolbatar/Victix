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

enum DragType {
    NONE,
    NORMAL,
    MINIMAP
};

class World {
private:
    WorldPosition state;
    ImVec2 last_drag;
    DragType dragging = DragType::NONE;
    float ff, left_edge, right_edge;

    // Smooth scroll animation
    double l_velocity = 0.0;
    double r_velocity = 0.0;
    double acceleration = 0.25;
    double deceleration = 0.5;
    double max_velocity = 15.0;

    // Box2D
    const float timeStep = 1.0f / 60.0f;
    const int32 velocityIterations = 6;
    const int32 positionIterations = 2;
    CairoDebugDraw cairoDebugDraw;
    std::list<std::unique_ptr<Object>> objects;

public:
    bool add_mode = false;
    int idx, idx1, idx2;

    World(float scale);

    void Build(cairo_t *cr);

    void PreRender(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height);

    void Process(cairo_t *cr);

    std::list<std::unique_ptr<Object>> &GetObjects() { return objects; }
};
