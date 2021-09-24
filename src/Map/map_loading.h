#pragma once

#include "Core/config.h"
#include "Core/mathutil.h"
#include "Core/enums.h"

#include "Map/map.h"
#include "Entities/entity.h"
#include "Entities/Subclasses/sheep.h"
#include "Entities/entity_management.h"
#include "Core/gamedata.h"

namespace LevelManager {
    /*
    Checks to see if the given location is valid for spawning entities
    Ensures tile is non-solid, non-liquid, and is not already occupied by an entity
    */
    bool IsSpawnable(Map* level, Entity** entities, int xPos, int yPos);

    // Unloads archivable entities for storing, useful for saving puzzles
    void UnloadArchivable(Map* map, Entity** entitites);
    // Load entities from map into the currently loaded entity list
    void LoadArchivable(Map* map, Entity** entities);

    /*
    Loads a new level

    - Saves any entities that are archivable to the current map
    - Deletes entities from level data if they are no longer present

    - Grabs new Map object
    - Loads any stored entities into the main entity list
    - Spawns sheep in
    */
    Map* LoadLevel(GameData* data, bool spawnSheep = false);
};