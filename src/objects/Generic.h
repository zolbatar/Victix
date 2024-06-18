#pragma once

#include "Object.h"

class Generic : public Object {
protected:

    bool Update() override;

public:
    Generic(float x, float y);
};