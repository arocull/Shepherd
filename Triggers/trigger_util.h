#pragma once

#include "Core/config.h"
#include "Entities/entity.h"
#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/crate.h"
#include "Entities/entity_management.h"
#include "Map/map.h"

// Checks to see if all crates within a level are ontop of pressure plates (might not be useable for more unique crate puzzles)
bool Trigger_Internal_CheckAllCrates(Entity* entities[], Map* map, int NumberOfEntities = MaxEntities);

// Shortcut for configuring torches, locks torches by default
Torch* Trigger_Internal_TorchSetup(Entity* torch, bool extinguishable = false, bool useable = false, bool glow = false, bool hasFire = true, bool hasFrost = false);

// Shortcut for configuring crates, makes them incineratable by default
Crate* Trigger_Internal_CrateSetup(Entity* crate, bool canIncinerate = true);

// Makes the given torch uninteractable, while also setting fire and glow to the puzzle status
void Trigger_Internal_DisplayPuzzleStatus_Torch(Entity* torch, bool puzzleStatus);