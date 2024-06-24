#include <include/core/SkPaint.h>
#include "Generic.h"
#include "../ui/Skia.h"

bool Generic::Update() {
    return Object::ImpactUpdate();
}

Generic::Generic(float x, float y, Player player) : Object(x, y, player) {
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.5f, 0.5f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Generic::Render() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    auto canvas = Skia::GetCanvas();
    canvas->save();
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setARGB(255, 0, 0, 0);
    auto rect = SkRect::MakeXYWH(x - 0.5, y - 0.5, 1.0, 1.0);
    canvas->drawRect(rect, paint);
    canvas->restore();
}
