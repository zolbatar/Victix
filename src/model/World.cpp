#include <random>
#include "imgui.h"
#include "World.h"
#include "../objects/Generic.h"
#include "../objects/Emplacement.h"
#include "../ui/Minimap.h"
#include "../ui/Interface.h"
#include "../ui/Skia.h"

std::unique_ptr<b2World> world;
std::unique_ptr<Terrain> terrain;
extern ImFont *font_regular;
extern ImFont *font_bold;

// FPS
auto lastTime = std::chrono::high_resolution_clock::now();
float fps = 0.0f;
int frameCount = 0;
auto startTime = std::chrono::high_resolution_clock::now();

void updateFPS();

World::World(float scale) {
    this->state.scale = scale;
    ImGuiIO &io = ImGui::GetIO();
    float half_width = (io.DisplaySize.x * Interface::GetDPIScaling() / 2) / state.scale;
    left_edge = -Terrain::F_TERRAIN_WIDTH / 2.0f + half_width;
    right_edge = Terrain::F_TERRAIN_WIDTH / 2.0f - half_width;
    state.offset_x = left_edge;
    state.offset_y = 0.0f;

    // Box2D
    b2Vec2 gravity(0.0f, -10.0f);
    world = std::make_unique<b2World>(gravity);
    terrain = std::make_unique<Terrain>();

    // Debug draw
    debugDraw = std::make_unique<SkiaDebugDraw>(Skia::GetCanvas());
    world->SetDebugDraw(debugDraw.get());
    debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit |
                        b2Draw::e_centerOfMassBit);
}

void World::Build() {
    std::vector<double> &heights = terrain->GetHeights();

    int idx = 32;
    float x = idx - Terrain::F_TERRAIN_WIDTH / 2;
    Emplacement::AddEmplacement(x, (float) heights[idx] + 5, true, Player::FRIENDLY);
}

void World::PreRender(float width, float height) {
    ImGuiIO &io = ImGui::GetIO();
    state.offset_y = 0;

    // Terrain
    auto canvas = Skia::GetCanvas();
    canvas->save();
    canvas->translate(io.DisplaySize.x - (state.offset_x * state.scale),
                      io.DisplaySize.y * Interface::GetDPIScaling() -
                      (float) Terrain::F_TERRAIN_HEIGHT * state.scale);
    canvas->scale(state.scale, -state.scale);
    terrain->RenderSkia(state, 255, false, false, SkColorSetARGB(255, 25, 25, 112));
    canvas->restore();

    // Objects
    canvas->save();
    canvas->translate(io.DisplaySize.x - (state.offset_x * state.scale),
                      io.DisplaySize.y * Interface::GetDPIScaling() -
                      (float) Terrain::F_TERRAIN_HEIGHT * state.scale);
    canvas->scale(state.scale, -state.scale);
    for (auto &obj: objects) {
        obj->Render();
    }
    canvas->restore();

    // Debug draw
    if (false) {
        canvas->save();
        canvas->translate(io.DisplaySize.x - (state.offset_x * state.scale),
                          io.DisplaySize.y * Interface::GetDPIScaling() -
                          (float) (Terrain::F_TERRAIN_HEIGHT) * state.scale);
        canvas->scale(state.scale, -state.scale);
        world->DebugDraw();
        canvas->restore();
    }

    // Minimap
    canvas->restore();
    RenderMinimap(state);

    ImGui::BeginChild("Position", ImVec2(640, 360), false,
                      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 253, 208, 255));
    ImGui::TextUnformatted("Credits: ");
    ImGui::PushFont(font_bold);
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(150.0f, ImGui::GetCursorScreenPos().y));
    ImGui::Text("%d", state.credits);
    ImGui::PopFont();
    ImGui::Text("Position: %.2f %.2f", state.offset_x, state.offset_y);
    ImGui::Text("Scale: %.2f", state.scale);
    ImGui::Text("Bodies: %d/%zu", world->GetBodyCount(), objects.size());
    ImVec2 pos = ImGui::GetMousePos();
    ImGui::Text("Mouse: %.2f %.2f", pos.x, pos.y);
    updateFPS();
    ImGui::PopStyleColor();
    ImGui::EndChild();
}

void World::Process() {
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();

    // Update
    std::vector<double> &heights = terrain->GetHeights();
    objects.remove_if([](std::unique_ptr<Object> &obj) { return obj->Update(); });
    world->Step(timeStep, velocityIterations, positionIterations);

    // Dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        if (!dragging) {
            if (MinimapCheckDrag(pos, state)) {
                dragging = DragType::MINIMAP;
            } else {
                dragging = DragType::NORMAL;
                last_drag = drag_delta;
            }
        } else {
            if (dragging == DragType::MINIMAP) {
                MinimapCheckClick(pos, state);
            } else {
                float drag_scale = 1.0f / state.scale * Interface::GetDPIScaling();
                state.offset_x -= (drag_delta.x - last_drag.x) * drag_scale;
                state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
                l_velocity = 0;
                r_velocity = 0;
                last_drag = drag_delta;
            }
        }
    } else {
        dragging = DragType::NONE;
    }

    // Click?
    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && add_mode) {
        add_mode = false;
        Emplacement::Clear();
    } else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        add_mode = true;
    } else if (add_mode && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        add_mode = false;
        float x = (pos.x - io.DisplaySize.x / 2) * Interface::GetDPIScaling() / state.scale + state.offset_x;
        float y = (io.DisplaySize.y - pos.y) * Interface::GetDPIScaling() / state.scale + state.offset_y;
        Emplacement::AddEmplacement(x, y, true, Player::FRIENDLY);
    }

    // Move?
    if (ImGui::IsKeyPressed(ImGuiKey_A, false)) {
        state.offset_x = right_edge;
        l_velocity = 0;
        r_velocity = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        state.offset_x = left_edge;
        l_velocity = 0;
        r_velocity = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        state.offset_x = 0;
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
    state.offset_x += (float) r_velocity;
    state.offset_x -= (float) l_velocity;

    // Click
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
        MinimapCheckClick(pos, state);
    }

    // Off-screen?
    if (state.offset_x < left_edge) {
        state.offset_x = left_edge;
        r_velocity = 0;
        l_velocity = 0;
    }
    if (state.offset_x > right_edge) {
        state.offset_x = right_edge;
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
    ImGui::TextUnformatted("FPS: ");
    ImGui::PushFont(font_bold);
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(150.0f, ImGui::GetCursorScreenPos().y));
    ImGui::Text("%.2f", fps);
    ImGui::PopFont();
}