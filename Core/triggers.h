#pragma once

#include "config.h"

#include "mathutil.h"

#include "Core/renderwindow.h"
#include "Map/map.h"
#include "Entities/entity.h"

// Triggered whenever the player steps on a trigger tile or a pressure plate is pressed
void Trigger_OnTile(RenderWindow* window, Map* map, Entity* entities[], int triggerID);

// Triggered whenever a new game starts--use for opening cinematic when very first area is loaded
void Trigger_GameStart(RenderWindow* window, Map* map, Entity* entities[]);

// Triggered whenever the player swings their staff--Allows unique functionality for specific maps and helps with tutorials
void Trigger_StaffSwing(RenderWindow* window, Map* map, Entity* entities[]);

// Triggered whenever a given area is loaded
// Note: Only triggered when the leveal is manually navigated to by player
void Trigger_LevelLoaded(RenderWindow* window, Map* map, Entity* entities[]);