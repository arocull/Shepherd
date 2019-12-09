#pragma once

#include "config.h"

#include "mathutil.h"

#include "Core/renderwindow.h"
#include "Map/map.h"
#include "Entities/entity.h"

void DoMapTrigger(RenderWindow* window, Map* map, Entity* entities[], int triggerID);