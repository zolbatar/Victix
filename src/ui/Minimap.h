#pragma once

#include "../model/World.h"

void RenderMinimap(WorldPosition &state);

void MinimapCheckClick(ImVec2 &pos, WorldPosition &state);

bool MinimapCheckDrag(ImVec2 &pos, WorldPosition &state);
