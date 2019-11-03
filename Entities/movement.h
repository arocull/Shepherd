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

// Push entity distance X and Y; stops upon running into wall or foreign entity
void Movement_ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy);

// Move player
void Movement_ShiftPlayer(Map* world, Entity* entities[MaxEntities], Shepherd* obj, int dx, int dy, int* worldX, int* worldY);