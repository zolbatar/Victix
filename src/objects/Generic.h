#pragma once

#include "Object.h"

class Generic : public Object {
protected:

    bool Update() override;

public:
    void Render(cairo_t *cr) override;

    Generic(float x, float y);
};