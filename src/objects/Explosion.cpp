#include <include/core/SkPaint.h>
#include "Explosion.h"
#include "../ui/Skia.h"

float explosion_radius = 25.0f;

enum CollisionCategory {
    CATEGORY_NONE = 0x0000,
    CATEGORY_DEFAULT = 0x0001,
    CATEGORY_IGNORE = 0x0002 // Category for fixtures to be ignored
};

Explosion::Explosion(float x, float y, Player player) : Object(x, y, player) {
    b2CircleShape circleShape;
    circleShape.m_radius = explosion_radius / 2.0f / Object::world_adjust;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.isSensor = true;
    fixtureDef.filter.categoryBits = CATEGORY_IGNORE; // Assign it to ignore category
    fixtureDef.filter.maskBits = CATEGORY_NONE;      // Collide with nothing
//    body->CreateFixture(&fixtureDef);
}

Type Explosion::GetType() {
    return Type::BOMB;
}

void Explosion::Render() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    auto canvas = Skia::GetCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setARGB(alpha > 255 ? 255 : alpha, 255, 255, 0);
    canvas->drawCircle(x, y, explosion_radius / 2.0f, paint);
}

bool Explosion::Update() {
    alpha -= 5;
    if (alpha == 0)
        return true;
    return false;
}

bool Explosion::ReadyToActivate() {
    return false;
}

void Explosion::Activate() {

}


