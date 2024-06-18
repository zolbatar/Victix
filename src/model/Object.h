#pragma once

#include <memory>
#include <box2d/box2d.h>
#include <cairo.h>

class Object {
private:
    b2Body *body = nullptr;

public:
    Object(float x, float y);

    Object(const Object&) = delete;

    ~Object();

    void Render(cairo_t *cr) const;

    bool Update(std::vector<double> &heights);
};
