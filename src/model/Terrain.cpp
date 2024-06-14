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
                    TERRAIN_WIDTH * pos.scale, TERRAIN_HEIGHT * pos.scale);
    double sky_shade = 0.7;
    cairo_set_source_rgb(cr, 27.0 / 100.0 * sky_shade, 56.0 / 100.0 * sky_shade, 89.0 / 100.0 * sky_shade);
    cairo_fill(cr);

    // Sea
    cairo_rectangle(cr,
                    pos.ConvertWorldToScreenX((-TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(0),
                    TERRAIN_WIDTH * pos.scale, TERRAIN_HEIGHT * pos.scale);
    cairo_set_source_rgb(cr, 0.0 / 100.0, 14.12 / 100.0, 21.96 / 100.0);
    cairo_fill(cr);

    // Sea line
    cairo_move_to(cr, pos.ConvertWorldToScreenX((-TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(0));
    cairo_line_to(cr, pos.ConvertWorldToScreenX((TERRAIN_WIDTH / 2)), pos.ConvertWorldToScreenY(0));
    sky_shade = 0.9;
    cairo_set_line_width(cr, 1.0 * (1.0 + (pos.scale * 5.0)));
    cairo_set_source_rgb(cr, 74.0 / 100.0 * sky_shade, 84.0 / 100.0 * sky_shade, 96.0 / 100.0 * sky_shade);
    cairo_stroke(cr);

    // Work out step
    unsigned int d = next_power_of_2(1.0f / pos.scale);

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
    cairo_set_source_rgb(cr, 24.41 / 100.0, 13.04 / 100.0, 7.55 / 100.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgba(cr, 19.61 / 100.0, 80.39 / 100.0, 19.61 / 100.0, 1.0);
    cairo_set_line_width(cr, 1.0 * (1.0 + (pos.scale * 5.0)));
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
