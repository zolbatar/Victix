#include <include/core/SkPaint.h>
#include <include/effects/SkImageFilters.h>
#include "Bomb.h"
#include "../ui/Skia.h"

float Bomb::size = 5.0f;

bool Bomb::Update() {
    return Object::ImpactUpdate();
}

Bomb::Bomb(float x, float y, Player player) : Object(x, y, player) {
    b2CircleShape circleShape;
    circleShape.m_radius = size / 2.0f;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    b2Vec2 initialVelocity(80.0f, 100.0f);
    body->SetLinearVelocity(initialVelocity);
}

void Bomb::Render() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    auto canvas = Skia::GetCanvas();
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kFill_Style);
    if (player == Player::FRIENDLY) {
        paint.setARGB(255, 125, 249, 255);
        sk_sp <SkImageFilter> dropShadow = SkImageFilters::DropShadow(
                0.0f, 0.0f,  // dx, dy
                2.0f, 2.0f,    // sigmaX, sigmaY
                SkColorSetARGB(255, 255, 255, 255), // shadow color
                nullptr        // input (nullptr means apply to the paint directly)
        );
        paint.setImageFilter(dropShadow);
    } else {
        paint.setARGB(255, 227, 66, 52);
        sk_sp <SkImageFilter> dropShadow = SkImageFilters::DropShadow(
                0.0f, 0.0f,  // dx, dy
                2.0f, 2.0f,    // sigmaX, sigmaY
                SkColorSetARGB(255, 255, 255, 255), // shadow color
                nullptr        // input (nullptr means apply to the paint directly)
        );
        paint.setImageFilter(dropShadow);
    }
    canvas->drawCircle(x, y, size / 2.0f, paint);
}

Type Bomb::GetType() {
    return Type::BOMB;
}

bool Bomb::ReadyToActivate() {
    return false;
}

void Bomb::Activate() {

}
