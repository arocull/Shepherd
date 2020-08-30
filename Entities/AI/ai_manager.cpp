#include "Entities/AI/ai_manager.h"

AIManager::AIManager() {
    shepherd = nullptr;
    level = nullptr;
    entities = nullptr;
    tick = 0;
    enemyGoal = nullptr;
}

// Sets the current context for the AI manager--also decides the current wolf target
void AIManager::SetContext(Entity* newShepherd, Map* currentLevel, Entity** levelEntities, int currentTick) {
    shepherd = newShepherd;
    level = currentLevel;
    entities = levelEntities;
    tick = currentTick;

    float avgWolfX = 0;
    float avgWolfY = 0;
    int numWolves = 0;

    // First, get average position of wolves (they should try to travel in a pack)
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() == EntityID::EE_Wolf) {
            avgWolfX += (float) levelEntities[i]->x;
            avgWolfY += (float) levelEntities[i]->y;
            numWolves++;
        }
    }

    // Do further AI calculations if wolves are present
    if (numWolves > 0 && shepherd) {
        avgWolfX /= numWolves;
        avgWolfY /= numWolves;

        Entity** sheep = (Entity**) calloc(MaxSheep, sizeof(Entity*));
        float* sheepPriorities = (float*) calloc(MaxSheep, sizeof(float));
        int currentSheep = 0;

        for (int i = 0; i < MaxEntities; i++) {
            if (levelEntities[i] && levelEntities[i]->GetID() == EntityID::EE_Sheep) {
                Entity* obj = levelEntities[i];
                sheep[currentSheep] = obj;

                float shepherdDist = (float) distGrid(obj->x, obj->y, shepherd->x, shepherd->y);

                // Sheep gain priority the further they are away from the shepherd, but lose some priority if they are further from the wolves
                // Priority is logarithmic, however, so sheep close to the shepherd gain much less priority than those far away
                sheepPriorities[currentSheep] = (
                    shepherdDist * log10(shepherdDist / 1.5) * 1.15
                    - distGrid((float) obj->x, (float) obj->y, avgWolfX, avgWolfY)
                    + 3 * (2 - obj->GetHealth()) // Harmed sheep also gain priority as well, but armored sheep lose priority
                );
                currentSheep++;
            }
        }

        // Then find the sheep with highest priority and select it for attack
        float minPriority = -100;
        for (int i = 0; i < MaxSheep; i++) {
            if (sheepPriorities[i] > minPriority) {
                enemyGoal = sheep[i];
                minPriority = sheepPriorities[i];
            }
        }
        // If the best target still has a low priority, target the shepherd instead
        if (minPriority <= 0) enemyGoal = shepherd;

        free(sheep);
        free(sheepPriorities);
    }
}
void AIManager::ClearContext() {
    shepherd = nullptr;
    level = nullptr;
    entities = nullptr;
    enemyGoal = nullptr;
}

// Ticks the given AI
void AIManager::TickAI(Entity* entity) {
    switch (entity->GetID()) {
        case EntityID::EE_Sheep: return TickSheep(dynamic_cast<Sheep*>(entity));
        case EntityID::EE_Wolf: return TickWolf(dynamic_cast<Wolf*>(entity));
        default: return;
    }
}
void AIManager::TickSheep(Sheep* sheep) {
    if (!((tick % 2) == 0 || distGrid(sheep->x, sheep->y, shepherd->x, shepherd->y) > 5)) return;
    sheep->animation = 0;
    
    int dirX = 0;
    int dirY = 0;

    CheckPathObscurity(sheep->currentPath, level, entities, true, EntityID::EE_Sheep);

    if (!sheep->currentPath || !sheep->currentPath->complete || !(sheep->currentPath->goalX == shepherd->x && sheep->currentPath->goalY == shepherd->y)) {
        Path_FreePath(sheep->currentPath);
        sheep->currentPath = GetPath(sheep->x, sheep->y, shepherd->x, shepherd->y, level, entities, sheep, true, true, EntityID::EE_Wolf);
    }

    GetNextMovement(sheep->x, sheep->y, sheep->currentPath, &dirX, &dirY);
    Movement_ShiftEntity(level, entities, sheep, dirX, -dirY);
}
void AIManager::TickWolf(Wolf* wolf) {
    if (wolf->InStun()) { // If the wolf is stunned, do not move
        return wolf->TickStun();
    }
    
    if (!wolf->IsHunting()) { // If the wolf is not in hunt mode, select a target and begin the hunt
        if (enemyGoal) wolf->InitiateHunt(enemyGoal);
        else wolf->animation = 0; // Idle stance if no target

        return;
    }

    Entity* target = wolf->GetTarget();

    if (distGrid(wolf->x, wolf->y, target->x, target->y) <= 1) { // Do attack
        target->TakeDamage(1, wolf);
        wolf->EndHunt(); // End hunt for target reselection (moves to new pack target)
    }

    int dirX = 0;
    int dirY = 0;

    CheckPathObscurity(wolf->currentPath, level, entities, false, EntityID::EE_Wolf);
    if (!wolf->currentPath || !wolf->currentPath->complete || !(wolf->currentPath->goalX == target->x && wolf->currentPath->goalY == target->y)) {
        Path_FreePath(wolf->currentPath);
        wolf->currentPath = GetPath(wolf->x, wolf->y, target->x, target->y, level, entities, wolf, false, true, EntityID::EE_Fireball);
    }
    GetNextMovement(wolf->x, wolf->y, wolf->currentPath, &dirX, &dirY);

    Movement_ShiftEntity(level, entities, wolf, dirX, -dirY);
    wolf->animation = 1; // Run animation
}