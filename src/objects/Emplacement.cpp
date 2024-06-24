#include <chrono>
#include "Emplacement.h"
#include "../model/World.h"
#include "Generic.h"

static std::vector<float> previous;
extern std::unique_ptr<World> game_world;
extern std::unique_ptr<Terrain> terrain;
float Emplacement::size = 15.0;

Emplacement::Emplacement(float x, float y, Player player) : Object(x, y, player) {
    b2Vec2 vertices[4];
    vertices[0].Set(size * 0.5f, -size * 0.5f);
    vertices[1].Set(size * 0.3f, size * 0.5f);
    vertices[2].Set(-size * 0.3f, size * 0.5f);
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
        if (obj->Type() == Type::EMPLACEMENT) {
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
            previous[i - game_world->idx1] = (float) heights[i];
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
}

Type Emplacement::Type() {
    return Type::EMPLACEMENT;
}

void Emplacement::RenderInternal(float x, float y, float a, Player player, bool outline, bool valid) {
    auto canvas = Skia::GetCanvas();
    canvas->save();

    // Position and rotation
    canvas->translate(x, y);
    canvas->rotate(a);
    canvas->translate(-x, -y);

    // Build path
    SkPath path;
    path.moveTo(x + size * 0.5, y - size * 0.5);
    path.lineTo(x + size * 0.3, y + size * 0.5);
    path.lineTo(x - size * 0.3, y + size * 0.5);
    path.lineTo(x - size * 0.5, y - size * 0.5);
    path.close();
    SkPath outline_path;
    outline_path = path;
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    if (!outline) {
        if (player == Player::FRIENDLY)
            paint.setARGB(255, 0, 0, 64);
        else
            paint.setARGB(255, 64, 0, 0);
        canvas->drawPath(path, paint);
    }

    // Outline
    if (player == Player::FRIENDLY)
        paint.setARGB(valid ? 255 : 128, 0, 0, 255);
    else
        paint.setARGB(valid ? 255 : 128, 255, 0, 0);
/*    if (outline) {
        // Define the dash pattern (dash length, gap length)
        double dashes[] = {1.0, 3.0};
        int num_dashes = sizeof(dashes) / sizeof(dashes[0]);
        auto time = std::chrono::system_clock::now(); // Get the current time
        auto since_epoch = time.time_since_epoch();   // Get the duration since epoch
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
        double offset = millis.count() / 100.0; // Start point of the dash pattern

        // Set the dash pattern
        cairo_set_dash(cr, dashes, num_dashes, offset);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    }*/
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeWidth(1.5);
    canvas->drawPath(outline_path, paint);
    canvas->restore();
}


