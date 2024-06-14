#include <random>
#include "Terrain.h"
#include "imgui.h"

unsigned int next_power_of_2(float x);
cairo_pattern_t *set_linear(double cx, double cy, double length, double angle);

std::random_device rd;  // Random device to seed the generator
std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()
std::uniform_real_distribution<> dis(0.0, 1.0); // Uniform distribution in the range [0, 1)

void Terrain::GenerateTerrain(PerlinNoise &perlin) {
    heights.resize(TERRAIN_WIDTH);
    float y = dis(gen) * TERRAIN_WIDTH;
    for (int x = 0; x < TERRAIN_WIDTH; ++x) {
        double height = perlin.noise(x, y, FREQ, DEPTH) * SCALE;
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
    unsigned int d = next_power_of_2(1.0f / scale_x);

    // Do it
    for (int i = 0; i < TERRAIN_WIDTH; i += d) {
        double this_x = i * scale_x + offset_x;
        double this_y = -heights[i] * scale_y + offset_y;
        if (i == 0)
            cairo_move_to(cr, this_x, this_y);
        else
            cairo_line_to(cr, this_x, this_y);
    }
    cairo_path_t *path = cairo_copy_path(cr); // Save for outline
    cairo_line_to(cr, offset_x + (scale_x * (TERRAIN_WIDTH - 1)), offset_y + (TERRAIN_HEIGHT / 2));
    cairo_line_to(cr, offset_x, offset_y + (TERRAIN_HEIGHT / 2));
    cairo_close_path(cr);

    // Fill
    cairo_pattern_t *pat = set_linear(TERRAIN_WIDTH * scale_x, TERRAIN_HEIGHT * 0.7, TERRAIN_HEIGHT, 270);


/*    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.0, 1.0, 0.0);
    cairo_pattern_add_color_stop_rgb(pat, 0.5, 1.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.0, 0.0, 1.0);*/

    cairo_pattern_add_color_stop_rgb(pat, 1.0, layer1[0], layer1[1], layer1[2]);
    cairo_pattern_add_color_stop_rgb(pat, 0.5, layer2[0], layer2[1], layer2[2]);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, layer3[0], layer3[1], layer3[2]);

    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // Outline
    cairo_append_path(cr, path);
    cairo_set_source_rgba(cr, 19.61 / 100.0, 80.39 / 100.0, 19.61 / 100.0, 1.0);
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

const double pi = 3.14159265358979323846;

double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

cairo_pattern_t *set_linear(double cx, double cy, double length, double angle) {
    // Define the angle in radians
    angle = degrees_to_radians(angle);

    // Calculate the start and end points of the gradient
    double x1 = cx - length * cos(angle) / 2;
    double y1 = cy - length * sin(angle) / 2;
    double x2 = cx + length * cos(angle) / 2;
    double y2 = cy + length * sin(angle) / 2;

    // Create a linear gradient pattern at the specified angle
    return cairo_pattern_create_linear(x1, y1, x2, y2);
}