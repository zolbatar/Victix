#pragma once

#include <memory>
#include <box2d/box2d.h>
#include "Types.h"
#include "Player.h"

class Object {
protected:
    b2Body *body = nullptr;
    float radius = 10.0f;
    float minimap_size = 5.0f;
    Player player;

public:
    Object(Player player);

    Object(float x, float y, Player player);

    Object(const Object &) = delete;

    virtual ~Object();

    virtual Type GetType() = 0;

    Player GetPlayer() { return player; };

    virtual void Render() = 0;

    virtual bool Update() = 0;

    virtual bool ReadyToActivate() = 0;

    virtual void Activate() = 0;

    b2Body *GetBody() { return body; }

    [[nodiscard]] float GetMinimapSize() const { return minimap_size; }

protected:
    bool ImpactUpdate();

};
