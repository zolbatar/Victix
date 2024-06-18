#pragma once

#include <cairo.h>
#include "../model/World.h"

void RenderMinimap(cairo_t *cr, std::vector<double> &heights, WorldPosition &pos);
