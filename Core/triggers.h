#pragma once

#include "config.h"

#include "mathutil.h"

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

// Triggered whenever the player steps on a trigger tile
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



// Checks to see if all crates within a level are ontop of pressure plates (might not be useable for more unique crate puzzles)
bool Trigger_Internal_CheckAllCrates(Entity* entities[], Map* map, int NumberOfEntities = MaxEntities);

// Shortcut for configuring torches, locks torches by default
Torch* Trigger_Internal_TorchSetup(Entity* torch, bool extinguishable = false, bool useable = false, bool glow = false, bool hasFire = true, bool hasFrost = false);

// Shortcut for configuring crates, makes them incineratable by default
Crate* Trigger_Internal_CrateSetup(Entity* crate, bool canIncinerate = true);

// Makes the given torch uninteractable, while also setting fire and glow to the puzzle status
void Trigger_Internal_DisplayPuzzleStatus_Torch(Entity* torch, bool puzzleStatus);


// Performs initial puzzle set up for the given map
void Trigger_SetupPuzzles(Map* map);