#pragma once

#include "Object.h"

class Explosion : public Object {
private:
    int alpha = 500;
public:

    Explosion(float x, float y, Player player);

    Type GetType() override;

    void Render() override;

    bool Update() override;

    bool ReadyToActivate() override;

    void Activate() override;
};

