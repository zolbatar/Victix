#include "Generic.h"

bool Generic::Update() {
    return Object::ImpactUpdate();
}

Generic::Generic(float x, float y) : Object(x, y) {
    // Nothing extra to do here
}
