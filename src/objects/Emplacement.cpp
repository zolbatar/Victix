#include "Emplacement.h"
#include "../model/World.h"
#include "Generic.h"

extern std::unique_ptr<World> game_world;
extern std::unique_ptr<Terrain> terrain;
float Emplacement::size = 15.0;

Emplacement::Emplacement(float x, float y) : Object(x, y) {
    b2Vec2 vertices[4];
    vertices[0].Set(size * 0.5, -size * 0.5);
    vertices[1].Set(size * 0.3, size * 0.5);
    vertices[2].Set(-size * 0.3, size * 0.5);
    vertices[3].Set(-size * 0.5, -size * 0.5);
    b2PolygonShape dynamicBox;
    dynamicBox.Set(vertices, 4);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
}

void Emplacement::AddEmplacement(float x, float y, WorldPosition &state) {

    // Get heights
    auto &heights = terrain->GetHeights();

    // Align X
    x = round(x);
    y = round(y);

    // Work out index into height array
    ImGuiIO &io = ImGui::GetIO();
    int idx = round(x + Terrain::F_TERRAIN_WIDTH / 2);
    int idx1 = round(idx - size / 2) - 2;
    int idx2 = round(idx + size / 2) + 2;

    for (int i = idx1; i < idx2; i++) {
        heights[i] = y - size / 2;
    }

    game_world->GetObjects().emplace_back(std::make_unique<Emplacement>(x, y + 5));
    terrain->UpdateBox2D();
}

bool Emplacement::Update() {
    return false;
}

void Emplacement::Render(cairo_t *cr) {
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;
    float a = body->GetAngle();
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
    if (is_player)
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.25);
    else
        cairo_set_source_rgb(cr, 0.25, 0.0, 0.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    if (is_player)
        cairo_set_source_rgb(cr, 0.0 / 255.0, 191.0 / 255.0, 255.0 / 255.0);
    else
        cairo_set_source_rgb(cr, 220.0 / 255.0, 20.0 / 255.0, 60.0 / 255.0);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);
    cairo_restore(cr);
}


