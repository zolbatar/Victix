#pragma once

#include <cairo.h>
#include "../model/World.h"

void RenderMinimap(cairo_t *cr, WorldPosition &state);

void MinimapCheckClick(ImVec2 &pos, WorldPosition &state);

bool MinimapCheckDrag(ImVec2 &pos, WorldPosition &state);
