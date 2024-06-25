#include <include/core/SkPaint.h>
#include <include/effects/SkImageFilters.h>
#include "Bomb.h"
#include "../ui/Skia.h"
#include "../model/Terrain.h"
#include "Explosion.h"

float Bomb::size = 5.0f;
extern std::unique_ptr<World> game_world;
extern std::unique_ptr<Terrain> terrain;

static std::random_device rd;  // Random device to seed the generator
static std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
static std::uniform_real_distribution<> dis(7.5, 15.0); // Uniform distribution in the range [0, 1)

bool Bomb::Update() {
    bool impact = Object::ImpactUpdate();
    float slow = body->GetLinearVelocity().Length();
    if (impact || slow < 5.0f) {
        float x = body->GetPosition().x;
        float y = body->GetPosition().y;

        // Explosion?
        int idx = round(x + Terrain::F_TERRAIN_WIDTH / 2);
        int idx1 = round(idx - size / 2) - 10;
        int idx2 = round(idx + size / 2) + 10;
        int indices = idx2 - idx1;

        // Sine wave
        float sine[indices];
        float step = M_PI / (indices - 1);
        int ii = 0;
        for (int xx = 0; xx <= indices; xx++) {
            float sine_value = sin(xx * step);
            sine[xx] = sine_value;
        }
        // Adjust
        auto &heights = terrain->GetHeights();
        float damage = dis(gen);
        for (int i = idx1; i < idx2; i++) {
            heights[i] -= damage * sine[i - idx1];
        }

        terrain->UpdateBox2D();

        // Create explosion
        game_world->GetObjects().emplace_back(std::make_unique<Explosion>(x, y, player));
    }
    return impact || slow < 5.0f;
}

Bomb::Bomb(float x, float y, Player player) : Object(x, y, player) {
    b2CircleShape circleShape;
    circleShape.m_radius = size / 2.0f;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    b2Vec2 initialVelocity(WorldPosition::shoot_delta_x, WorldPosition::shoot_delta_y);
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
        sk_sp<SkImageFilter> dropShadow = SkImageFilters::DropShadow(
                0.0f, 0.0f,  // dx, dy
                2.0f, 2.0f,    // sigmaX, sigmaY
                SkColorSetARGB(255, 255, 255, 255), // shadow color
                nullptr        // input (nullptr means apply to the paint directly)
        );
        paint.setImageFilter(dropShadow);
    } else {
        paint.setARGB(255, 227, 66, 52);
        sk_sp<SkImageFilter> dropShadow = SkImageFilters::DropShadow(
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
