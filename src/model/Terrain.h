#pragma once

#include <vector>
#include <cairo.h>
#include "PerlinNoise.h"

class Terrain
{
 private:
	const int TERRAIN_HEIGHT = 500;
	const int TERRAIN_WIDTH = 8192;
	const float SCALE = 2.5f;
	const float FREQ = 0.0005f;
	const int DEPTH = 3;
	std::vector<float> heights;

 public:
	Terrain();
	void Render(cairo_t* cr);
	void GenerateTerrain(PerlinNoise& perlin);
};
