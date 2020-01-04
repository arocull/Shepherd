#pragma once

#include "config.h"

#include "mathutil.h"

#include "Core/renderwindow.h"
#include "Audio/sound_service.h"

#include "Map/map.h"
#include "Entities/entity.h"

// Triggered whenever the player steps on a trigger tile or a pressure plate is pressed
void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID);

// Triggered whenever a new game starts--use for opening cinematic when very first area is loaded
void Trigger_GameStart(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered whenever the player swings their staff--Allows unique functionality for specific maps and helps with tutorials
void Trigger_StaffSwing(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered whenever the player idles for TicksUntilIdle amount of ticks
void Trigger_Idled(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered whenever a given area is loaded
// Note: Only triggered when the leveal is manually navigated to by player
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);