#pragma once

#include "Core/config.h"
#include "Core/enums.h"
#include "Core/mathutil.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/sheep.h"
#include "Entities/Subclasses/wolf.h"

#include "Map/map.h"
#include "Entities/movement.h"
#include "Entities/AI/path_manager.h"

// AI Manager - Takes a context and makes decisions for given entities, including movement, attacks, and interactions
class AIManager {
    public:
        AIManager();
    
    private:
        Entity* shepherd;
        Map* level;
        Entity** entities;
        int tick;

        Entity* enemyGoal;

        void TickSheep(Sheep* sheep);
        void TickWolf(Wolf* wolf);

    public:
        void SetContext(Entity* shepherd, Map* currentLevel, Entity** levelEntities, int currentTick);
        void ClearContext();

        void TickAI(Entity* entity);
};