#include "Generic.h"

bool Generic::Update() {
    return Object::ImpactUpdate();
}

Generic::Generic(float x, float y) : Object(x, y) {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.5f, 0.5f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Generic::Render(cairo_t *cr) {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle(cr, x - 0.5, y - 0.5, 1.0, 1.0);
//    cairo_arc(cr, x, y, 1.0, 0, 2 * M_PI);
    cairo_fill(cr);
}
