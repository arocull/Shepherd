#pragma once

#include <cstdlib>

#include "Core/config.h"
#include "Core/mathutil.h"

#include "Map/map.h"
#include "Entities/entity.h"
#include "Entities/Subclasses/shepherd.h"
#include "Core/gamedata.h"
#include "Entities/entity_management.h"

// Movement - Used for storing info on player movement and moving entities in general
// TODO: Code needs some serious refactoring
// TODO: Move player data out of here?
namespace Movement {
    extern bool MoveUp;
    extern bool MoveDown;
    extern bool MoveRight;
    extern bool MoveLeft;
    extern bool Move_QueueClear;
    extern bool MoveFireballQueued;

    // Clears all queued player movement
    void ClearQueue();

    // Checks to see if there is anything solid at the given position
    bool CheckCollision(Map* world, Entity* entities[MaxEntities], int x, int y);
    // Checks to see if there is anything solid at the given position
    // - Will attempt to push sheep and crates out of the way
    bool CheckCollisionWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy);

    // Checks to see if there is any solid entities at the given position or if the tiles fit a certain ID or are pressure plates
    bool CheckCollisionOnTiles(Map* world, Entity* entities[MaxEntities], int x, int y, int tileID);
    // Checks to see if there is any solid entities at the given position or if the tiles fit a certain ID or are pressure plates
    // - Will attempt to push sheep and crates out of the way
    bool CheckCollisionOnTilesWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy, int tileID);

    // Push entity distance X and Y; stops upon running into wall or foreign entity 
    // - Returns true if object made it to destination--false otherwise
    bool ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, bool doPush = false);

    // Push entity distance X and Y; refuses to go anywhere but designated tile ID or pressure plates; stops upon running into solid entities
    // - Returns true if object made it to destination--false otherwise
    bool ShiftEntityOnTiles(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, int tileID, bool doPush = false);

    // Move player; interacts with entities it collides with as well
    void ShiftPlayer(GameData* data, int dx, int dy);
}