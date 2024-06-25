#include <random>
#include "imgui.h"
#include "World.h"
#include "../objects/Bomb.h"
#include "../objects/Emplacement.h"
#include "../ui/Minimap.h"
#include "../ui/Interface.h"
#include "../ui/Skia.h"

std::unique_ptr<b2World> world;
std::unique_ptr<Terrain> terrain;
extern ImFont *font_regular;
extern ImFont *font_bold;

int WorldPosition::credits = 5000;
int WorldPosition::cost = 0;
float WorldPosition::shoot_delta_x;
float WorldPosition::shoot_delta_y;

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

void World::Build(int number) {
    std::vector<double> &heights = terrain->GetHeights();

    // Players
    int idx = 50;
    for (int i = 0; i < number; i++) {
        float x = idx - Terrain::F_TERRAIN_WIDTH / 2;
        float y = (float) heights[idx] + 15;
        y += Terrain::TERRAIN_HEIGHT;
        Emplacement::AddEmplacement(x, y, true, Player::FRIENDLY);
        idx += 50;
    }

    idx = Terrain::TERRAIN_WIDTH - 50;
    for (int i = 0; i < number; i++) {
        float x = idx - Terrain::F_TERRAIN_WIDTH / 2;
        float y = (float) heights[idx] + 15;
        y += Terrain::TERRAIN_HEIGHT;
        Emplacement::AddEmplacement(x, y, true, Player::ENEMY);
        idx -= 50;
    }

    WorldPosition::credits = 5000;
    WorldPosition::cost = -1;
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
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(2.5f);
    paint.setARGB(255, 255, 255, 0);
    for (auto &obj: objects) {
        obj->Render();
        if (obj->ReadyToActivate() && mode == Mode::BOMBARD) {
            auto pos = obj->GetBody()->GetPosition();
            SkPath path;
            path.moveTo(pos.x, pos.y + Emplacement::size);
            path.lineTo(pos.x + WorldPosition::shoot_delta_x, pos.y + WorldPosition::shoot_delta_y);

            // Shade it
            SkColor colors[] = {SK_ColorTRANSPARENT, SK_ColorYELLOW};
            SkPoint points[] = {SkPoint::Make(pos.x, pos.y + Emplacement::size),
                                SkPoint::Make(pos.x + WorldPosition::shoot_delta_x,
                                              pos.y + WorldPosition::shoot_delta_y)};
            sk_sp<SkShader> shader = SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp);
            paint.setShader(shader);

            // Draw as arrow
            Interface::DrawArrowLine(points[0], points[1], 5.0f, paint);
        }
    }
    canvas->restore();

    // Debug draw
    if (false) {
        canvas->save();
        canvas->translate(io.DisplaySize.x - (state.offset_x * state.scale),
                          io.DisplaySize.y * Interface::GetDPIScaling() -
                          (float) (Terrain::F_TERRAIN_HEIGHT) * state.scale);
        canvas->scale(state.scale * Object::world_adjust, -state.scale * Object::world_adjust);
        world->DebugDraw();
        canvas->restore();
    }

    // Minimap
    canvas->restore();
    RenderMinimap(state);

    ImGui::BeginChild("Position", ImVec2(640, 360), false,
                      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 253, 208, 255));

    // Credits
    ImGui::TextUnformatted("Credits: ");
    ImGui::PushFont(font_bold);
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(150.0f, ImGui::GetCursorScreenPos().y));
    ImGui::Text("%d", state.credits);
    ImGui::PopFont();

    // Mode
    ImGui::TextUnformatted("Mode: ");
    ImGui::PushFont(font_bold);
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(150.0f, ImGui::GetCursorScreenPos().y));
    switch (mode) {
        case Mode::ADD:
            ImGui::TextUnformatted("Adding Emplacement");
            break;
        case Mode::BOMBARD:
            ImGui::TextUnformatted("Bombard");
            break;
        case Mode::FLAK:
            ImGui::TextUnformatted("Flak");
            break;
        default:
            ImGui::TextUnformatted("None");
            break;
    }
    ImGui::PopFont();

    // Cost (if appropriate)
    if (WorldPosition::cost >= 0) {
        ImGui::TextUnformatted("Cost: ");
        ImGui::PushFont(font_bold);
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(150.0f, ImGui::GetCursorScreenPos().y));
        ImGui::Text("%d", WorldPosition::cost);
        ImGui::PopFont();
    }

/*    ImGui::Text("Position: %.2f %.2f", state.offset_x, state.offset_y);
    ImGui::Text("Scale: %.2f", state.scale);
    ImGui::Text("Bodies: %d/%zu", world->GetBodyCount(), objects.size());
    ImVec2 pos = ImGui::GetMousePos();
    ImGui::Text("Mouse: %.2f %.2f", pos.x, pos.y);*/
    updateFPS();
    ImGui::PopStyleColor();
    ImGui::EndChild();
}

void World::Process() {
    auto canvas = Skia::GetCanvas();
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();
    float drag_scale = 1.0f / state.scale * Interface::GetDPIScaling();

    canvas->save();
    canvas->translate(io.DisplaySize.x - (state.offset_x * state.scale),
                      io.DisplaySize.y * Interface::GetDPIScaling() -
                      (float) Terrain::F_TERRAIN_HEIGHT * state.scale);
    canvas->scale(state.scale, -state.scale);

    // Update
    objects.remove_if([](std::unique_ptr<Object> &obj) { return obj->Update(); });
    world->Step(timeStep, velocityIterations, positionIterations);

    // Dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        if (mode == Mode::NONE) {
            if (MinimapCheckDrag(pos, state)) {
                mode = Mode::MINIMAP;
            } else {
                mode = Mode::NORMAL;
                last_drag = drag_delta;
            }
        } else {
            if (mode == Mode::MINIMAP) {
                MinimapCheckClick(pos, state);
            } else {
                state.offset_x -= (drag_delta.x - last_drag.x) * drag_scale;
                state.offset_y += (drag_delta.y - last_drag.y) * drag_scale;
                l_velocity = 0;
                r_velocity = 0;
                last_drag = drag_delta;
            }
        }
    } else if (mode == Mode::MINIMAP || mode == Mode::NORMAL) {
        mode = Mode::NONE;
    }

    // Click?
    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && mode == Mode::ADD) {
        mode = Mode::NONE;
        Emplacement::Clear();
    } else if (mode == Mode::NONE && ImGui::IsKeyPressed(ImGuiKey_A, false)) {
        mode = Mode::ADD;
    } else if (mode == Mode::ADD && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
               WorldPosition::credits >= WorldPosition::cost) {
        mode = Mode::NONE;
        float x = (pos.x - io.DisplaySize.x / 2) * Interface::GetDPIScaling() / state.scale + state.offset_x;
        float y = (io.DisplaySize.y - pos.y) * Interface::GetDPIScaling() / state.scale + state.offset_y;
        Emplacement::AddEmplacement(x, y, true, Player::FRIENDLY);
    } else if (mode == Mode::ADD) {
        float x = (pos.x - io.DisplaySize.x / 2) * Interface::GetDPIScaling() / state.scale + state.offset_x;
        float y = (io.DisplaySize.y - pos.y) * Interface::GetDPIScaling() / state.scale + state.offset_y;
        Emplacement::AddEmplacement(x, y, false, Player::FRIENDLY);
    }

    // Shoot?
    if (mode == Mode::NONE && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        mode = Mode::BOMBARD;
        last_drag = drag_delta;
        WorldPosition::shoot_delta_x = 0;
        WorldPosition::shoot_delta_y = 0;
    } else if (mode == Mode::BOMBARD && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        WorldPosition::shoot_delta_x += (drag_delta.x - last_drag.x) * drag_scale;
        WorldPosition::shoot_delta_y -= (drag_delta.y - last_drag.y) * drag_scale;
        last_drag = drag_delta;

        // Keep sensible
        if (WorldPosition::shoot_delta_x < 25) WorldPosition::shoot_delta_x = 25;
        if (WorldPosition::shoot_delta_x > 200) WorldPosition::shoot_delta_x = 200;
        if (WorldPosition::shoot_delta_y < 0) WorldPosition::shoot_delta_y = 0;
        if (WorldPosition::shoot_delta_y > 100) WorldPosition::shoot_delta_y = 100;

    } else if (mode == Mode::BOMBARD && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {

        // Fire?
        for (auto &obj: this->GetObjects()) {
            if (obj->GetType() == Type::EMPLACEMENT && obj->ReadyToActivate() && obj->GetPlayer() == Player::FRIENDLY) {
                obj->Activate();
            }
        }

        mode = Mode::NONE;
    }

    // Move?
    if (ImGui::IsKeyPressed(ImGuiKey_C, false)) {
        state.offset_x = right_edge;
        l_velocity = 0;
        r_velocity = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        state.offset_x = left_edge;
        l_velocity = 0;
        r_velocity = 0;
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, false)) {
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
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right, false)) {
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
    canvas->restore();
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