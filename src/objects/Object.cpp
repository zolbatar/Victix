#include <cmath>
#include <cassert>
#include "Object.h"
#include "../model/Terrain.h"

extern std::unique_ptr<b2World> world;
extern std::unique_ptr<Terrain> terrain;

Object::Object(float x, float y, Player player) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
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
    int xa = x + Terrain::F_TERRAIN_WIDTH / 2.0;
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
