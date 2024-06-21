#include <random>
#include "imgui.h"
#include "World.h"
#include "../objects/Generic.h"
#include "../objects/Emplacement.h"
#include "../ui/Minimap.h"
#include "../ui/Interface.h"

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
    cairoDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit |
                            b2Draw::e_centerOfMassBit);
    world->SetDebugDraw(&cairoDebugDraw);
}

void World::Build(cairo_t *cr) {
    std::vector<double> &heights = terrain->GetHeights();

    int idx = 32;
    float x = idx - Terrain::F_TERRAIN_WIDTH / 2;
    Emplacement::AddEmplacement(cr, x, (float) heights[idx] + 5, true, Player::FRIENDLY);
}

void World::PreRender(cairo_t *cr, cairo_surface_t *surface, GLuint render, float width, float height) {
    ImGuiIO &io = ImGui::GetIO();
    cairoDebugDraw.SetCR(cr);
    state.offset_y = 0;

    // Clear the surface with a transparent color
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0); // Fully transparent
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);

    // Terrain
    cairo_save(cr);
    cairo_translate(cr,
                    io.DisplaySize.x - (state.offset_x * state.scale),
                    io.DisplaySize.y + (2 * Terrain::F_TERRAIN_HEIGHT * state.scale));
    cairo_scale(cr, state.scale, -state.scale);
    terrain->Render(cr, state);
    terrain->RenderSkia(state);

    // Objects
    for (auto &obj: objects) {
        obj->Render(cr);
    }

    // Render debug draw using Cairo
/*    cairo_set_line_width(cr, 0.1);
    world->DebugDraw();*/

    // Minimap
    cairo_restore(cr);
    RenderMinimap(cr, state);

    // Write to texture and blit
    cairo_surface_flush(surface);
    glBindTexture(GL_TEXTURE_2D, render);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 cairo_image_surface_get_data(surface));

    // Blur
//    auto out = Interface::DoBlur(render, width, height);
    ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(render),
            ImVec2(0.0f, 0.0f),
            ImVec2(width, height),
            ImVec2(0.0f, 0.0f),
            ImVec2(Interface::GetDPIScaling(), Interface::GetDPIScaling()));

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

void World::Process(cairo_t *cr) {
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
        Emplacement::AddEmplacement(cr, x, y, true, Player::FRIENDLY);
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