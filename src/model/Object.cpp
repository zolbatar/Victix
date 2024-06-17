#include "Object.h"

Object::Object(const std::shared_ptr<b2World> &world, float x, float y) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    body = world->CreateBody(&bodyDef);
    dynamicBox.SetAsBox(1.0f, 1.0f);
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Object::Render(cairo_t *cr) const {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_arc(cr, x, y, 1.0, 0, 2 * M_PI);
    cairo_fill(cr);
}