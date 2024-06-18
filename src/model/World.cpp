#include <random>
#include "imgui.h"
#include "World.h"

std::unique_ptr<b2World> world;
static std::random_device rd;  // Random device to seed the generator
static std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
static std::uniform_real_distribution<> dis(0.0, 150.0);
static std::uniform_int_distribution<> disi(0, Terrain::TERRAIN_WIDTH);

World::World() {
    ImGuiIO &io = ImGui::GetIO();
    state.zoom = 0;
    state.scale = state.zooms[state.zoom];
    state.offset_x = 0.0f;
    state.offset_y = 0.0f;

    // Box2D
    b2Vec2 gravity(0.0f, -10.0f);
    world = std::make_unique<b2World>(gravity);

    // Ground
    b2BodyDef groundBodyDef;
    groundBodyDef.type = b2_staticBody;
    groundBodyDef.position.Set(0.0f, 0.0f);
    groundBody = world->CreateBody(&groundBodyDef);

    // Ground shape
    std::vector<double> &heights = terrain.GetHeights();
    int hsize = (int) heights.size();
    float hsize_half = heights.size() / 2;
    b2Vec2 vertices[hsize + 2];
    for (int i = 0; i < heights.size(); i++) {
        float x = i - hsize_half;
        vertices[hsize - i - 1].Set(x, heights[i]);
    }
    vertices[heights.size()].Set(-hsize_half, -Terrain::TERRAIN_HEIGHT);
    vertices[heights.size() + 1].Set(hsize_half, -Terrain::TERRAIN_HEIGHT);
    b2ChainShape groundBox;
    groundBox.CreateLoop(vertices, hsize + 2);
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Body
    for (unsigned int i = 0; i < 250; i++) {
        int idx = disi(gen);
        int x = idx - Terrain::TERRAIN_WIDTH / 2;
        objects.emplace_back(x, (float) heights[idx] + 5 + dis(gen));
    }

    // Debug draw
    cairoDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit |
                            b2Draw::e_centerOfMassBit);
    world->SetDebugDraw(&cairoDebugDraw);
}

void World::Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height) {
    cairoDebugDraw.SetCR(cr);

    // Background
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Terrain
    cairo_save(cr);
    ImGuiIO &io = ImGui::GetIO();
    cairo_translate(cr,
                    io.DisplaySize.x / 2 - (state.offset_x * state.scale),
                    io.DisplaySize.y / 2 + (state.offset_y * state.scale));
    cairo_scale(cr, state.scale, -state.scale);
    terrain.Render(cr, state);

    // Objects
    for (auto &obj: objects) {
        obj.Render(cr);
    }

    // Render debug draw using Cairo
/*    cairo_set_line_width(cr, 0.1);
    world->DebugDraw();*/
    cairo_restore(cr);

    // Write to texture and blit
    cairo_surface_flush(surface);
    glBindTexture(GL_TEXTURE_2D, render);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 cairo_image_surface_get_data(surface));
    ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(render),
            ImVec2(0.0f, 0.0f),
            ImVec2(width, height),
            ImVec2(0.0f, 0.0f),
            ImVec2(1.0f, 1.0f),
            IM_COL32(255, 255, 255, 255));

    ImGui::BeginChild("Position", ImVec2(200, 200), false,
                      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    ImGui::Text("Position: %.2f %.2f", state.offset_x, state.offset_y);
    ImGui::Text("Scale: %.2f %d", state.scale, state.zoom);
    ImGui::Text("Bodies: %d/%zu", world->GetBodyCount(), objects.size());
    ImGui::PopStyleColor();
    ImGui::EndChild();
}

void World::DoZoom(int vzoom) {
    state.zoom += vzoom;
    if (state.zoom < 0)
        state.zoom = 0;
    if (state.zoom >= state.count_zooms)
        state.zoom = state.count_zooms - 1;
    state.scale = state.zooms[state.zoom];
}

void World::Process() {
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();

    // Update
    std::vector<double> &heights = terrain.GetHeights();
    objects.remove_if([&heights](Object &obj) {
        return obj.Update(heights);
    });
    world->Step(timeStep, velocityIterations, positionIterations);

    // Zoom
    if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        DoZoom(1);
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, false)) {
        DoZoom(-1);
    } else if (io.MouseWheel != 0.0f) {
        if (io.MouseWheel < 0)
            DoZoom(-1);
        else
            DoZoom(1);
    }

    // Dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        if (!dragging) {
            dragging = true;
            last_drag = drag_delta;
        } else {
            float drag_scale = 1.0 / state.scale;
            state.offset_x -= (drag_delta.x - last_drag.x) * drag_scale;
            state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
            last_drag = drag_delta;
        }
    } else {
        dragging = false;
    }
}
