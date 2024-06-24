#pragma once

#include "Object.h"

class Bomb : public Object {
protected:
    static float size;

    bool Update() override;

public:
    void Render() override;

    bool ReadyToActivate() override;

    void Activate() override;

    Type GetType() override;

    Bomb(float x, float y, Player player);
};