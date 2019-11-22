#pragma once

#include "Core/config.h"
#include "Core/mathutil.h"

#include "Map/map.h"

#include "Entities/entity.h"
#include "Entities/entity_management.h"

// Checks to see if the given location is valid for spawning entities
bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos);

/* Loads a new level
Saves any entities that are archivable to the current level
Deletes entities from level data if they are no longer present */
Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY);

Map* GenerateMapFromPNG();