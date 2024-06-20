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

void Emplacement::AddEmplacement(cairo_t *cr, float x, float y, bool final, Player player) {
    auto &heights = terrain->GetHeights();

    // Align X
    x = round(x);
    y = round(y);

    // Work out index into height array
    ImGuiIO &io = ImGui::GetIO();
    game_world->idx = round(x + Terrain::F_TERRAIN_WIDTH / 2);

    // Work out height
    float height = y - heights[game_world->idx];

    // Make sure height is valid
    if (height > 50)
        y = heights[game_world->idx] + 50;
    else if (height < -10)
        y = heights[game_world->idx] - 10;

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

                Emplacement::RenderInternal(cr, x, y, 0, player, true, false);
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

        Emplacement::RenderInternal(cr, x, y, 0, player, true, true);
    }
}

bool Emplacement::Update() {
    return false;
}

void Emplacement::Render(cairo_t *cr) {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    float a = body->GetAngle();
    RenderInternal(cr, x, y, a, player, false, true);
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

void Emplacement::RenderInternal(cairo_t *cr, float x, float y, float a, Player player, bool outline, bool valid) {
    cairo_save(cr);
    cairo_translate(cr, x, y);
    cairo_rotate(cr, a);
    cairo_translate(cr, -x, -y);
    cairo_move_to(cr, x + size * 0.5, y - size * 0.5);
    cairo_line_to(cr, x + size * 0.3, y + size * 0.5);
    cairo_line_to(cr, x - size * 0.3, y + size * 0.5);
    cairo_line_to(cr, x - size * 0.5, y - size * 0.5);
    cairo_close_path(cr);
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    if (!outline) {
        if (player == Player::FRIENDLY)
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.25);
        else
            cairo_set_source_rgb(cr, 0.25, 0.0, 0.0);
        cairo_fill(cr);
    }

    // Outline
    cairo_append_path(cr, path);
    if (player == Player::FRIENDLY)
        cairo_set_source_rgba(cr, 0.0 / 255.0, 191.0 / 255.0, 255.0 / 255.0, valid ? 1.0 : 0.5);
    else
        cairo_set_source_rgba(cr, 220.0 / 255.0, 20.0 / 255.0, 60.0 / 255.0, valid ? 1.0 : 0.5);
    if (outline) {
        // Define the dash pattern (dash length, gap length)
        double dashes[] = {1.0, 1.0};
        int num_dashes = sizeof(dashes) / sizeof(dashes[0]);
        double offset = 0.0; // Start point of the dash pattern

        // Set the dash pattern
        cairo_set_dash(cr, dashes, num_dashes, offset);
    }
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);
    cairo_restore(cr);
}


