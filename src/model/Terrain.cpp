#include "Terrain.h"
#include "imgui.h"

unsigned int next_power_of_2(float x);

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double height = perlin.noise(x * SCALE, 0.0, 0.1, 5);
        heights[x] = (float) height;
    }
}

Terrain::Terrain() {
    PerlinNoise noise;
    GenerateTerrain(noise);
}

void Terrain::Render(cairo_t *cr) {
    ImGuiIO &io = ImGui::GetIO();

    // Terrain scales etc;
    float scale_x = 0.1f;
    float scale_y = TERRAIN_HEIGHT / 4;
    float offset_x = io.DisplaySize.x / 2 - (TERRAIN_WIDTH / 2 * scale_x);
    float offset_y = TERRAIN_HEIGHT / 2;

    // Work out step
    uint d = next_power_of_2(1.0f / scale_x);

    // Do it
    for (int i = 0; i < TERRAIN_WIDTH; i += d) {
        if (i == 0) {
            cairo_move_to(cr, i * scale_x + offset_x, heights[i] * scale_y + offset_y);
        } else {
            cairo_line_to(cr, i * scale_x + offset_x, heights[i] * scale_y + offset_y);
        }
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, offset_x + (scale_x * (TERRAIN_WIDTH - 1)), offset_y + (TERRAIN_HEIGHT / 2));
    cairo_line_to(cr, offset_x, offset_y + (TERRAIN_HEIGHT / 2));
    cairo_close_path(cr);

    // Fill
    cairo_pattern_t *pat = cairo_pattern_create_linear(0, 0, 0, TERRAIN_HEIGHT / 2);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.0, 1.0, 0.0);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.0, 0.0, 1.0);
    cairo_set_source(cr, pat);
//    cairo_set_source_rgba(cr, 0.25, 0.25, 0.25, 1.0);
    cairo_fill(cr);

    // Outler line
    cairo_append_path(cr, path);
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
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