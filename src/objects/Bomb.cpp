#include <include/core/SkPaint.h>
#include "Bomb.h"
#include "../ui/Skia.h"

bool Bomb::Update() {
    return Object::ImpactUpdate();
}

Bomb::Bomb(float x, float y, Player player) : Object(x, y, player) {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.5f, 0.5f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Bomb::Render() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    auto canvas = Skia::GetCanvas();
    canvas->save();
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setARGB(255, 0, 0, 0);
    auto rect = SkRect::MakeXYWH(x - 0.5f, y - 0.5f, 1.0f, 1.0f);
    canvas->drawRect(rect, paint);
    canvas->restore();
}

Type Bomb::GetType() {
    return Type::BOMB;
}

bool Bomb::ReadyToActivate() {
    return false;
}

void Bomb::Activate() {

}
