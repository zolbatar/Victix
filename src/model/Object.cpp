#include <cmath>
#include "Object.h"
#include "Terrain.h"

Object::Object(const std::shared_ptr<b2World> &world, float x, float y) : world(world) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    body = world->CreateBody(&bodyDef);
    dynamicBox.SetAsBox(0.5f, 0.5f);
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

Object::~Object() {
    world->DestroyBody(body);
}

bool Object::Update(std::vector<double> &heights) {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;

    // Impact?
    int xa = x + Terrain::F_TERRAIN_WIDTH / 2.0;
    float diff = fabs(y - (float) heights[xa]);
    if (diff < 5.0) {
        return true;
    }

    if (x < -Terrain::F_TERRAIN_WIDTH / 2 || x >= Terrain::F_TERRAIN_WIDTH / 2)
        return true;
    if (y < -Terrain::F_TERRAIN_HEIGHT / 2)
        return true;
    return false;
}

void Object::Render(cairo_t *cr) const {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);\
    cairo_rectangle(cr, x - 0.5, y - 0.5, 1.0, 1.0);
//    cairo_arc(cr, x, y, 1.0, 0, 2 * M_PI);
    cairo_fill(cr);
}