#pragma once

#include <list>
#include <memory>
#include "Terrain.h"
#include "WorldPosition.h"
#include <box2d/box2d.h>
#include "SkiaDebugDraw.h"
#include "../objects/Object.h"

enum class Mode {
    NONE,
    NORMAL,
    MINIMAP,
    ADD,
    BOMBARD,
    FLAK
};

class World {
private:
    WorldPosition state;
    ImVec2 last_drag;
    std::unique_ptr<SkiaDebugDraw> debugDraw;
    Mode mode = Mode::NONE;
    float left_edge, right_edge;

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
    std::list<std::unique_ptr<Object>> objects;

public:
    int idx, idx1, idx2;

    World(float scale);

    void Build(int number);

    void PreRender(float width, float height);

    void Process();

    std::list<std::unique_ptr<Object>> &GetObjects() { return objects; }

    WorldPosition &GetState() { return state; }
};
