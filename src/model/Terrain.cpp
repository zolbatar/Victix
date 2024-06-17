#include <random>
#include "Terrain.h"
#include "imgui.h"
#include "../ui/Interface.h"

unsigned int next_power_of_2(float x);

static std::random_device rd;  // Random device to seed the generator
static std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
static std::uniform_real_distribution<> dis(0.0, Terrain::TERRAIN_WIDTH); // Uniform distribution in the range [0, 1)

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    double y = dis(gen);
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
    cairo_rectangle(cr, -F_TERRAIN_WIDTH / 2.0, -F_TERRAIN_HEIGHT, F_TERRAIN_WIDTH, F_TERRAIN_HEIGHT * 2.0);
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_fill(cr);

    // Clip to render area
    cairo_rectangle(cr, -F_TERRAIN_WIDTH / 2.0, -F_TERRAIN_HEIGHT, F_TERRAIN_WIDTH, F_TERRAIN_HEIGHT * 2.0);
    cairo_clip(cr);

    // Work out step
    unsigned int d = next_power_of_2(1.0f / pos.scale);

    // Do it
    for (unsigned int i = 0; i < F_TERRAIN_WIDTH; i += d) {
        if (i == 0)
            cairo_move_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
        else
            cairo_line_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, (F_TERRAIN_WIDTH / 2), F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -(F_TERRAIN_WIDTH / 2), F_TERRAIN_HEIGHT);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1.0);
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
