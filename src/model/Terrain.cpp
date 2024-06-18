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

    // Do it
    for (unsigned int i = 0; i < F_TERRAIN_WIDTH; i++) {
        if (i == 0)
            cairo_move_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
        else
            cairo_line_to(cr, i - F_TERRAIN_WIDTH / 2, heights[i]);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, (F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    cairo_line_to(cr, -(F_TERRAIN_WIDTH / 2), -F_TERRAIN_HEIGHT);
    cairo_close_path(cr);

    // Fill
    cairo_set_source_rgb(cr, 25.0 / 255.0, 25.0 / 255.0, 112.0 / 255.0);
    cairo_fill(cr);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgb(cr, 0.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
    cairo_set_line_width(cr, 1.5);
    cairo_stroke(cr);
}
