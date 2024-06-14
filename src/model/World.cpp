#include "World.h"

void World::Render(cairo_t *cr) {

    // Background
    cairo_set_source_rgb(cr, 0.05, 0.05, 0.1);
    cairo_paint(cr);

    // Terrain
    terrain.Render(cr);

/*	PerlinNoise noise;
	terrain.GenerateTerrain(noise);*/
}
