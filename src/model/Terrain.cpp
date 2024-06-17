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
        heights[x] = perlin.noise(x, y, FREQ, DEPTH) * SCALE * TERRAIN_HEIGHT;
    }
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::Render(cairo_t *cr, WorldPosition &pos) {
    ImGuiIO &io = ImGui::GetIO();

    // Sky
    cairo_rectangle(cr, -TERRAIN_WIDTH / 2, -TERRAIN_HEIGHT, TERRAIN_WIDTH, TERRAIN_HEIGHT * 2);
    double sky_shade = 0.7;
    cairo_set_source_rgb(cr, 27.0 / 100.0 * sky_shade, 56.0 / 100.0 * sky_shade, 89.0 / 100.0 * sky_shade);
    cairo_fill(cr);

    // Work out step
    unsigned int d = next_power_of_2(1.0f / pos.scale);

    // Do it
    for (int i = 0; i < TERRAIN_WIDTH; i += d) {
        if (i == 0)
            cairo_move_to(cr, i - Terrain::TERRAIN_WIDTH / 2, heights[i]);
        else
            cairo_line_to(cr, i - Terrain::TERRAIN_WIDTH / 2, heights[i]);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, (TERRAIN_WIDTH / 2), -TERRAIN_HEIGHT);
    cairo_line_to(cr, -(TERRAIN_WIDTH / 2), -TERRAIN_HEIGHT);
    cairo_close_path(cr);

    // Fill
    cairo_set_source_rgb(cr, 24.41 / 100.0, 13.04 / 100.0, 7.55 / 100.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgba(cr, 19.61 / 100.0, 80.39 / 100.0, 19.61 / 100.0, 1.0);
    cairo_set_line_width(cr, 0.25);
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
