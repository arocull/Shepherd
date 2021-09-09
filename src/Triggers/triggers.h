#pragma once

#include "Core/config.h"

#include "Core/mathutil.h"

#include "Core/renderwindow.h"
#include "Audio/sound_service.h"

#include "Entities/entity_management.h"

#include "Map/map.h"
#include "Map/puzzle.h"

#include "Entities/particle.h"
#include "Entities/entity.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/crate.h"
#include "Entities/Subclasses/lever.h"
#include "Entities/Boss/pyramidgolem.h"

#include "Triggers/trigger_util.h"
#include "Triggers/scripts_ontile.h"
#include "Triggers/scripts_puzzleinput.h"

// (Trigger) Init - Initializes script function pointer arrays
void Trigger_Init(int maxMapID);
// (Trigger) Free - Deallocates script function pointer arrays
void Trigger_Free();

// (Trigger) On Tile - Triggered whenever the player steps on a trigger tile
void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID);

// Triggered whenever the player steps on a scroll (reads scroll off and performs unique actions if necessary)
void Trigger_OnScroll(RenderWindow* window, SoundService* soundService, Map* map,  Entity* entities[]);

// Triggered whenever the player steps on a fizzler (extinguishes fire/frost flames and performs unique actions if necessary)
void Trigger_OnFizzler(RenderWindow* window, SoundService* soundService, Map* map,  Entity* shepherd);

// Triggered whenever a new game starts--use for opening cinematic when very first area is loaded
void Trigger_GameStart(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered when a game over occurs--freezes all entities and forces the shepherd to lay down
void Trigger_GameOver(RenderWindow* window, SoundService* SoundService, Map* map, Entity* entities[]);

// Triggered whenever the player swings their staff--Allows unique functionality for specific maps and helps with tutorials
void Trigger_StaffSwing(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered whenever the player idles for TicksUntilIdle amount of ticks
void Trigger_Idled(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);

// Triggered whenever the pressure plate states of a level changes, a torch is lit or extinguished, or a lever is flipped
void Trigger_PuzzleInput(RenderWindow* window, SoundService* SoundService, Particle* particles, Map* map, Entity* entities[]);

// Triggered whenever a given area is loaded
// Allows access to whole world to check for other puzzles being solved
// Note: Only triggered when the level is manually navigated to by the player
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* world[WorldWidth][WorldHeight], Map* map, Entity* entities[]);

// Triggered when the current level's event timer reaches zero
void Trigger_LevelEvent(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]);



// (Trigger) Setup Puzzles - Performs initial puzzle set up for the given map
void Trigger_SetupPuzzles(Map* map);