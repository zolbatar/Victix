#pragma once

#include <memory>
#include <box2d/box2d.h>
#include <cairo.h>
#include "../model/Terrain.h"

class Object {
protected:
    b2Body *body = nullptr;
    float radius = 1.5f;

public:
    Object(float x, float y);

    Object(const Object &) = delete;

    virtual ~Object();

    void Render(cairo_t *cr) const;

    virtual bool Update() = 0;

protected:
    bool ImpactUpdate();

};
