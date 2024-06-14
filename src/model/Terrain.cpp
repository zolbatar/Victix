#include <random>
#include "Terrain.h"
#include "imgui.h"
#include "../ui/Interface.h"

unsigned int next_power_of_2(float x);

std::random_device rd;  // Random device to seed the generator
std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
std::uniform_real_distribution<> dis(0.0, 1.0); // Uniform distribution in the range [0, 1)

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    float y = dis(gen) * TERRAIN_WIDTH;
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double height = perlin.noise(x, y, FREQ, DEPTH) * SCALE * TERRAIN_HEIGHT;
        heights[x] = (float) height;
    }
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::Render(cairo_t *cr, WorldPosition &pos) {
    ImGuiIO &io = ImGui::GetIO();

    // Debug centre crosshair
/*    cairo_move_to(cr, 0, io.DisplaySize.y / 2);
    cairo_line_to(cr, io.DisplaySize.x, io.DisplaySize.y / 2);
    cairo_move_to(cr, io.DisplaySize.x / 2, 0);
    cairo_line_to(cr, io.DisplaySize.x / 2, io.DisplaySize.y);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_stroke(cr);*/

    // Sky
    cairo_rectangle(cr,
                    pos.ConvertWorldToScreenX((-TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(TERRAIN_HEIGHT),
                    TERRAIN_WIDTH * pos.scale_x, TERRAIN_HEIGHT * pos.scale_y);
    cairo_pattern_t *pat = Interface::SetLinear(pos.ConvertWorldToScreenX(TERRAIN_WIDTH), pos.ConvertWorldToScreenY(TERRAIN_HEIGHT / 2),
                                                TERRAIN_HEIGHT * pos.scale_y / 2, 90);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, sky1[0], sky1[1], sky1[2]);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, sky3[0], sky3[1], sky3[2]);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // Sea
    cairo_rectangle(cr,
                    pos.ConvertWorldToScreenX((-TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(0),
                    TERRAIN_WIDTH * pos.scale_x, TERRAIN_HEIGHT * pos.scale_y);
    pat = Interface::SetLinear(pos.ConvertWorldToScreenX(TERRAIN_WIDTH), pos.ConvertWorldToScreenY(TERRAIN_HEIGHT / 2),
                               TERRAIN_HEIGHT * pos.scale_y / 2, 270);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, sea1[0], sea1[1], sea1[2]);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, sea3[0], sea3[1], sea3[2]);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // Work out step
    unsigned int d = next_power_of_2(1.0f / pos.scale_x);

    // Do it
    for (int i = 0; i < TERRAIN_WIDTH; i += d) {
        double this_x = pos.ConvertWorldToScreenX(i - Terrain::TERRAIN_WIDTH / 2);
        double this_y = pos.ConvertWorldToScreenY(heights[i]);
        if (i == 0)
            cairo_move_to(cr, this_x, this_y);
        else
            cairo_line_to(cr, this_x, this_y);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, pos.ConvertWorldToScreenX((TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(-TERRAIN_HEIGHT));
    cairo_line_to(cr, pos.ConvertWorldToScreenX(-(TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(-TERRAIN_HEIGHT));
    cairo_close_path(cr);

    // Fill
    pat = Interface::SetLinear(pos.ConvertWorldToScreenX(TERRAIN_WIDTH), pos.ConvertWorldToScreenY(TERRAIN_HEIGHT * 0.2),
                               TERRAIN_HEIGHT * pos.scale_y, 90);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, layer1[0], layer1[1], layer1[2]);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, layer3[0], layer3[1], layer3[2]);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgba(cr, 19.61 / 100.0, 80.39 / 100.0, 19.61 / 100.0, 1.0);
    cairo_set_line_width(cr, 1.0 * (1.0 + (pos.scale_x * 5.0)));
    cairo_stroke(cr);
    cairo_path_destroy(path);
}

unsigned int next_power_of_2(float x) {
    // Convert float to unsigned int for bit manipulation
    auto n = (unsigned int) ceil(x);

    // Check if n is already a power of 2
    if ((n & (n - 1)) == 0) {
        return n; // n is already a power of 2
    }

    // Find the next power of 2 using bit manipulation
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}
