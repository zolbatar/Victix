#pragma once

#include <memory>
#include <box2d/box2d.h>
#include <cairo.h>

class Object {
private:
    b2BodyDef bodyDef;
    b2PolygonShape dynamicBox;
    b2FixtureDef fixtureDef;
    b2Body *body;

public:
    Object(const std::shared_ptr<b2World> &world, float x, float y);

    void Render(cairo_t *cr) const;
};
