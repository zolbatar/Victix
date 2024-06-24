#include <chrono>
#include <include/effects/SkImageFilters.h>
#include "Emplacement.h"
#include "../model/World.h"
#include "Bomb.h"

static std::vector<float> previous;
extern std::unique_ptr <World> game_world;
extern std::unique_ptr <Terrain> terrain;
float Emplacement::size = 15.0f;

Emplacement::Emplacement(float x, float y, Player player) : Object(x, y, player) {
    b2Vec2 vertices[4];
    vertices[0].Set(size * 0.5f, -size * 0.5f);
    vertices[1].Set(size * 0.2f, size * 1.2f);
    vertices[2].Set(-size * 0.2f, size * 1.2f);
    vertices[3].Set(-size * 0.5f, -size * 0.5f);
    b2PolygonShape dynamicBox;
    dynamicBox.Set(vertices, 4);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Emplacement::AddEmplacement(float x, float y, bool final, Player player) {
    auto &heights = terrain->GetHeights();

    // Any previous?
    if (!previous.empty())
        Restore();

    // Align X
    ImGuiIO &io = ImGui::GetIO();
    x = round(x);
    y = round(y);

    // Work out index into height array
    game_world->idx = round(x + Terrain::F_TERRAIN_WIDTH / 2);

    // Work out height
    y -= Terrain::TERRAIN_HEIGHT;
    float height = y - heights[game_world->idx];

    // Make sure height is valid
    if (height > Terrain::TERRAIN_HEIGHT)
        y = heights[game_world->idx] + Terrain::TERRAIN_HEIGHT;
    else if (height < -Terrain::TERRAIN_HEIGHT / 4)
        y = heights[game_world->idx] - Terrain::TERRAIN_HEIGHT / 4;

    // Range
    game_world->idx1 = round(game_world->idx - size / 2) - 4;
    game_world->idx2 = round(game_world->idx + size / 2) + 4;
    int indices = game_world->idx2 - game_world->idx1;

    // Do we have an existing one, and do we have one close enough?
    for (auto &obj: game_world->GetObjects()) {
        if (obj->GetType() == Type::EMPLACEMENT) {
            float x_diff = abs(x - obj->GetBody()->GetPosition().x);
            if (x_diff < 50) {

                // Save previous
                previous.reserve(indices);
                for (int i = game_world->idx1; i < game_world->idx2; i++) {
                    previous[i - game_world->idx1] = (float) heights[i];
                }

                Emplacement::RenderInternal(x, y, 0, player, true, false);
                return;
            }
        }
    }

    // Actually place?
    if (final) {
        for (int i = game_world->idx1; i < game_world->idx2; i++) {
            heights[i] = y - size / 2;
        }
        game_world->GetObjects().emplace_back(std::make_unique<Emplacement>(x, y + 10, player));
        terrain->UpdateBox2D();
    } else {
        // Save previous
        previous.reserve(indices);
        for (int i = game_world->idx1; i < game_world->idx2; i++) {
            previous.push_back((float) heights[i]);
        }

        // Now update
        for (int i = game_world->idx1; i < game_world->idx2; i++) {
            heights[i] = y - size / 2;
        }

        Emplacement::RenderInternal(x, y, 0, player, true, true);
    }
}

bool Emplacement::Update() {
    return false;
}

void Emplacement::Render() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    float a = body->GetAngle();
    RenderInternal(x, y, a, player, false, true);
}

void Emplacement::Clear() {
    previous.clear();
}

void Emplacement::Restore() {
    auto &heights = terrain->GetHeights();
    for (int i = game_world->idx1; i < game_world->idx2; i++) {
        heights[i] = previous[i - game_world->idx1];
    }
    previous.clear();
}

Type Emplacement::GetType() {
    return Type::EMPLACEMENT;
}

void Emplacement::RenderInternal(float x, float y, float a, Player player, bool outline, bool valid) {
    auto canvas = Skia::GetCanvas();
    canvas->save();

    // Rotation
    canvas->translate(x, y);
    canvas->rotate(a);
    canvas->translate(-x, -y);

    SkPath path;
    path.moveTo(x + size * 0.25f, y - size * 0.5f);
    path.lineTo(x + size * 0.2f, y + size * 0.3f);
    path.lineTo(x, y + size * 0.5f);
    path.lineTo(x - size * 0.2f, y + size * 0.3f);
    path.lineTo(x - size * 0.25f, y - size * 0.5f);
    path.close();
    SkPath outline_path;
    outline_path = path;

    // Fill
    if (!outline) {

        // Fill
        SkPaint paint;
        paint.setStyle(SkPaint::Style::kFill_Style);
        paint.setAntiAlias(true);
        paint.setARGB(255, 0, 0, 0);
        canvas->drawPath(path, paint);

        // Glow
        path.reset();
        path.addCircle(x, y + size * 0.95f, size * 0.25f);
        if (player == Player::FRIENDLY) {
            paint.setARGB(255, 125, 249, 255);
            sk_sp <SkImageFilter> dropShadow = SkImageFilters::DropShadow(
                    0.0f, 0.0f,  // dx, dy
                    5.0f, 5.0f,    // sigmaX, sigmaY
                    SkColorSetARGB(255, 255, 255, 255), // shadow color
                    nullptr        // input (nullptr means apply to the paint directly)
            );
            paint.setImageFilter(dropShadow);
        } else {
            paint.setARGB(255, 227, 66, 52);
            sk_sp <SkImageFilter> dropShadow = SkImageFilters::DropShadow(
                    0.0f, 0.0f,  // dx, dy
                    5.0f, 5.0f,    // sigmaX, sigmaY
                    SkColorSetARGB(255, 255, 255, 255), // shadow color
                    nullptr        // input (nullptr means apply to the paint directly)
            );
            paint.setImageFilter(dropShadow);
        }
        canvas->drawPath(path, paint);
    }

    // Outline
    SkPaint paint;
    if (player == Player::FRIENDLY)
        paint.setARGB(valid ? 255 : 48, 135, 206, 250);
    else
        paint.setARGB(valid ? 255 : 48, 255, 0, 255);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeMiter(0.5f);
    paint.setStrokeJoin(SkPaint::Join::kRound_Join);
    paint.setStrokeWidth(1.0f);
    if (outline) {
        const SkScalar intervals[] = {2.0f, 3.0f};
        int count = sizeof(intervals) / sizeof(intervals[0]);
        SkScalar phase = (Skia::GetFrame() % 100) * 0.1f; // Adjust 0.5f for speed of walking
        sk_sp <SkPathEffect> dashEffect = SkDashPathEffect::Make(intervals, count, phase);
        paint.setPathEffect(dashEffect);
    }
    canvas->drawPath(outline_path, paint);
    canvas->restore();
}

void Emplacement::Activate() {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    game_world->GetObjects().emplace_back(std::make_unique<Bomb>(x, y + size, player));
}

bool Emplacement::ReadyToActivate() {
    return true;
}


