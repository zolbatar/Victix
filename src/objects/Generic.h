#pragma once

#include "Object.h"

class Generic : public Object {
protected:

    bool Update() override;

public:
    void Render() override;

    Generic(float x, float y, Player player);
};