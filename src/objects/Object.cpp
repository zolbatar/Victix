#include <cmath>
#include <cassert>
#include "Object.h"
#include "../model/Terrain.h"

extern std::unique_ptr<b2World> world;
extern std::unique_ptr<Terrain> terrain;

float Object::world_adjust = 1.0f;

Object::Object(Player player) {
    this->player = player;
}

Object::Object(float x, float y, Player player) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / world_adjust, y / world_adjust);
    body = world->CreateBody(&bodyDef);
    this->player = player;
}

Object::~Object() {
    if (body != nullptr && world != nullptr) {
        world->DestroyBody(body);
    }
}

bool Object::ImpactUpdate() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;

    // Sky?
    if (y >= Terrain::F_TERRAIN_HEIGHT * 5)
        return true;

    // Impact?
    int xa = round(x + Terrain::F_TERRAIN_WIDTH / 2.0f);
    float diff = fabs(y - (float) terrain->GetHeights()[xa]);
    if (diff < radius) {
        return true;
    }

    if (x < -Terrain::F_TERRAIN_WIDTH / 2 || x >= Terrain::F_TERRAIN_WIDTH / 2)
        return true;
    if (y < -Terrain::F_TERRAIN_HEIGHT / 2)
        return true;
    return false;
}
