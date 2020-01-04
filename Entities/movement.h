#pragma once

#include <cstdlib>

#include "Core/config.h"
#include "Core/mathutil.h"

#include "Map/map.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/shepherd.h"
#include "Entities/entity_management.h"


extern bool MoveUp;
extern bool MoveDown;
extern bool MoveRight;
extern bool MoveLeft;
extern bool Move_QueueClear;
extern bool MoveFireballQueued;

// Clears all queued player movement
void Movement_Clear();



// Checks to see if there is anything solid at the given position
bool Movement_CheckCollision(Map* world, Entity* entities[MaxEntities], int x, int y);

// Checks to see if there is any solid entities at the given position or if the tiles fit a certain ID or are pressure plates
bool Movement_CheckCollisionOnTiles(Map* world, Entity* entities[MaxEntities], int x, int y, int tileID);


// Push entity distance X and Y; stops upon running into wall or foreign entity
void Movement_ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy);

// Push entity distance X and Y; refuses to go anywhere but designated tile ID or pressure plates; stops upon running into solid entities
// - Returns true of object made it to destination--false otherwise
bool Movement_ShiftEntityOnTiles(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, int tileID);

// Move player; interacts with entities it collides with as well
void Movement_ShiftPlayer(Map* world, Entity* entities[MaxEntities], Shepherd* obj, int dx, int dy, int* worldX, int* worldY);