#include <random>
#include "imgui.h"
#include "World.h"
#include "../objects/Generic.h"
#include "../objects/MachineGun.h"
#include "../ui/Minimap.h"

Terrain terrain;
std::unique_ptr<b2World> world;
static std::random_device rd;  // Random device to seed the generator
static std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
static std::uniform_real_distribution<> dis(0.0, 150.0);
static std::uniform_int_distribution<> disi(0, Terrain::TERRAIN_WIDTH);

// FPS
auto lastTime = std::chrono::high_resolution_clock::now();
float fps = 0.0f;
int frameCount = 0;
auto startTime = std::chrono::high_resolution_clock::now();

void updateFPS();

World::World() {
    ImGuiIO &io = ImGui::GetIO();
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
        objects.emplace_back(std::make_unique<Generic>(x, (float) heights[idx] + 5 + dis(gen)));
    }

    // Add emplacements
    int idx = disi(gen);
    int x = idx - Terrain::TERRAIN_WIDTH / 2;
    objects.emplace_back(std::make_unique<MachineGun>(x, (float) heights[idx] + 5 + dis(gen)));

    // Debug draw
    cairoDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit |
                            b2Draw::e_centerOfMassBit);
    world->SetDebugDraw(&cairoDebugDraw);
}

void World::Render(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height) {
    ImGuiIO &io = ImGui::GetIO();
    cairoDebugDraw.SetCR(cr);
    switch (state.zoom) {
        case 0:
            state.scale = io.DisplaySize.y / (Terrain::F_TERRAIN_HEIGHT * 2);
            state.offset_y = 0;//Terrain::F_TERRAIN_HEIGHT;
            break;
        case 1:
            state.scale = io.DisplaySize.y / (Terrain::F_TERRAIN_HEIGHT * 6);
            state.offset_y = Terrain::F_TERRAIN_HEIGHT * 2;
            break;
    }

    // Clear the surface with a transparent color
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0); // Fully transparent
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);

    // Terrain
    cairo_save(cr);
    cairo_translate(cr,
                    io.DisplaySize.x / 2 - (state.offset_x * state.scale),
                    io.DisplaySize.y / 2 + (state.offset_y * state.scale));
    cairo_scale(cr, state.scale, -state.scale);
    terrain.Render(cr, state);

    // Objects
    for (auto &obj: objects) {
        obj->Render(cr);
    }

    // Minimap
    cairo_restore(cr);
    RenderMinimap(cr, terrain.GetHeights(), state);

    // Render debug draw using Cairo
/*    cairo_set_line_width(cr, 0.1);
    world->DebugDraw();*/

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
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 253, 208, 255));
    ImGui::Text("Position: %.2f %.2f", state.offset_x, state.offset_y);
    ImGui::Text("Scale: %.2f", state.scale);
    ImGui::Text("Bodies: %d/%zu", world->GetBodyCount(), objects.size());
    updateFPS();
    ImGui::PopStyleColor();
    ImGui::EndChild();
}

// Easing function: easeInOutQuad
// t: current time (in range [0, duration])
// b: start value
// c: change in value (end value - start value)
// d: total duration
double easeInOutQuad(double t, double b, double c, double d) {
    t /= d / 2;
    if (t < 1) {
        return c / 2 * std::pow(t, 2) + b;
    }
    t--;
    return -c / 2 * (t * (t - 2) - 1) + b;
}

void World::Process() {
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();

    // Update
    std::vector<double> &heights = terrain.GetHeights();
    objects.remove_if([](std::unique_ptr<Object> &obj) { return obj->Update(); });
    world->Step(timeStep, velocityIterations, positionIterations);

    // Zoom
    if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        state.zoom = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, false)) {
        state.zoom = 1;
/*    } else if (ImGui::IsKeyPressed(ImGuiKey_C, false)) {
        state.zoom = 2;*/
    } else if (io.MouseWheel != 0.0f) {
        if (io.MouseWheel < 0) {
            state.zoom--;
            if (state.zoom < 0)
                state.zoom = 0;
        } else {
            state.zoom++;
            if (state.zoom > 1)
                state.zoom = 1;
        }
    }

    // Dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        if (!dragging) {
            dragging = true;
            last_drag = drag_delta;
        } else {
            float drag_scale = 1.0f / state.scale;
            state.offset_x -= (drag_delta.x - last_drag.x) * drag_scale;
            state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
            l_velocity = 0;
            r_velocity = 0;
            last_drag = drag_delta;
        }
    } else {
        dragging = false;
    }

    // Bounds
    float ff = (io.DisplaySize.x / 2) / state.scale;
    float left_edge = (float) Terrain::F_TERRAIN_WIDTH / 2.0f - ff;
    float right_edge = (float) -Terrain::F_TERRAIN_WIDTH / 2.0f + ff;

    // Move?
    if (ImGui::IsKeyPressed(ImGuiKey_A, false)) {
        state.offset_x = right_edge;
        l_velocity = 0;
        r_velocity = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        state.offset_x = left_edge;
        l_velocity = 0;
        r_velocity = 0;
    }
    bool left_pressed = ImGui::IsKeyDown(ImGuiKey_LeftArrow);
    bool right_pressed = ImGui::IsKeyDown(ImGuiKey_RightArrow);

    // Animate?
    if (right_pressed) {
        r_velocity += acceleration;
        if (r_velocity > max_velocity)
            r_velocity = max_velocity;
    } else {
        if (r_velocity > 0) {
            r_velocity -= deceleration;
            if (r_velocity < 0)
                r_velocity = 0;
        } else {
            r_velocity += deceleration;
            if (r_velocity > 0)
                r_velocity = 0;
        }
    }
    if (left_pressed) {
        l_velocity += acceleration;
        if (l_velocity > max_velocity)
            l_velocity = max_velocity;
    } else {
        if (l_velocity > 0) {
            l_velocity -= deceleration;
            if (l_velocity < 0)
                l_velocity = 0;
        } else {
            l_velocity += deceleration;
            if (l_velocity > 0)
                l_velocity = 0;
        }
    }
    state.offset_x += r_velocity;
    state.offset_x -= l_velocity;

    // Off-screen?
    if (state.offset_x < -left_edge) {
        state.offset_x = -left_edge;
        r_velocity = 0;
        l_velocity = 0;
    }
    if (state.offset_x > -right_edge) {
        state.offset_x = -right_edge;
        r_velocity = 0;
        l_velocity = 0;
    }
}


void updateFPS() {
    // Get current time
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - lastTime;
    lastTime = currentTime;

    // Calculate FPS
    frameCount++;
    std::chrono::duration<float> totalTime = currentTime - startTime;
    if (totalTime.count() >= 1.0f) { // Update FPS every second
        fps = frameCount / totalTime.count();
        frameCount = 0;
        startTime = currentTime;
    }

    // Display FPS in ImGui
    ImGui::Text("FPS: %.2f", fps);
}