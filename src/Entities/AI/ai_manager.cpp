#include "Entities/AI/ai_manager.h"

AIManager::AIManager(GameData* newData, RenderWindow* newWindow, SoundService* newAudio) {
    data = newData;
    window = newWindow;
    audio = newAudio;
}

// Sets the current context for the AI manager--also decides the current wolf target
void AIManager::UpdateContext() {
    Shepherd* shepherd = data->player;
    Map* level = data->map;
    Entity** entities = data->entities;
    int tick = data->ticks;

    float avgWolfX = 0;
    float avgWolfY = 0;
    int numWolves = 0;

    // First, get average position of wolves (they should try to travel in a pack)
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->GetID() == EntityID::EE_Wolf) {
            avgWolfX += (float) entities[i]->x;
            avgWolfY += (float) entities[i]->y;
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
            if (entities[i] && entities[i]->GetID() == EntityID::EE_Sheep) {
                Entity* obj = entities[i];
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

// Ticks the given AI
void AIManager::TickAI(Entity* entity) {
    switch (entity->GetID()) {
        case EntityID::EE_Sheep: return TickSheep(dynamic_cast<Sheep*>(entity));
        case EntityID::EE_Wolf: return TickWolf(dynamic_cast<Wolf*>(entity));
        case EntityID::EE_Fireball: return TickFireball(dynamic_cast<Fireball*>(entity));

        // BOSS
        case EntityID::EE_PyramidGolem: return TickPyramidGolem(dynamic_cast<PyramidGolem*>(entity));
        default: return;
    }
}
void AIManager::TickSheep(Sheep* sheep) {
    if (!((data->ticks % 2) == 0 || distGrid(sheep->x, sheep->y, data->player->x, data->player->y) > 5)) return;
    sheep->animation = AnimationID::ANIM_Idle;
    
    int dirX = 0;
    int dirY = 0;

    CheckPathObscurity(sheep->currentPath, data->map, data->entities, true, EntityID::EE_Sheep);

    if (!sheep->currentPath || !sheep->currentPath->complete || !(sheep->currentPath->goalX == data->player->x && sheep->currentPath->goalY == data->player->y)) {
        Path_FreePath(sheep->currentPath);
        sheep->currentPath = GetPath(sheep->x, sheep->y, data->player->x, data->player->y, data->map, data->entities, sheep, true, true, EntityID::EE_Wolf);
    }

    GetNextMovement(sheep->x, sheep->y, sheep->currentPath, &dirX, &dirY);
    Movement::ShiftEntity(data->map, data->entities, sheep, dirX, -dirY);
}
void AIManager::TickWolf(Wolf* wolf) {
    if (wolf->InStun()) { // If the wolf is stunned, do not move
        return wolf->TickStun();
    }
    
    if (!wolf->IsHunting()) { // If the wolf is not in hunt mode, select a target and begin the hunt
        if (enemyGoal) wolf->InitiateHunt(enemyGoal);
        else wolf->animation = AnimationID::ANIM_Idle; // Idle stance if no target

        return;
    }

    Entity* target = wolf->GetTarget();

    if (distGrid(wolf->x, wolf->y, target->x, target->y) <= 1) { // Do attack
        target->TakeDamage(1, wolf);
        wolf->EndHunt(); // End hunt for target reselection (moves to new pack target)
    }

    int dirX = 0;
    int dirY = 0;

    CheckPathObscurity(wolf->currentPath, data->map, data->entities, false, EntityID::EE_Wolf);
    if (!wolf->currentPath || !wolf->currentPath->complete || !(wolf->currentPath->goalX == target->x && wolf->currentPath->goalY == target->y)) {
        Path_FreePath(wolf->currentPath);
        wolf->currentPath = GetPath(wolf->x, wolf->y, target->x, target->y, data->map, data->entities, wolf, false, true, EntityID::EE_Fireball);
    }
    GetNextMovement(wolf->x, wolf->y, wolf->currentPath, &dirX, &dirY);

    Movement::ShiftEntity(data->map, data->entities, wolf, dirX, -dirY);
    wolf->animation = AnimationID::ANIM_Walk; // Run animation
}
void AIManager::TickFireball(Fireball* fireball) {
    int fX = fireball->x;
    int fY = fireball->y;
    Movement::ShiftEntity(data->map, data->entities, fireball, fireball->speedX, fireball->speedY);

    // Set things on fire that the fireball is ontop of

    if (fireball->x == fX && fireball->y == fY) {     // Destroy fireball if it did not move
        fireball->Burst(data->entities, data->particles);
        window->AddScreenShake(0.15f, 0.0f);
    }
}

// BOSS AI //
void AIManager::TickPyramidGolem(PyramidGolem* boss) {
    Movement::ShiftEntity(data->map, data->entities, boss, boss->GetGoalX(), boss->GetGoalY(), true);
    boss->Flipped = false;

    if (boss->DoFireballToss()) {
        Fireball* fireball = new Fireball(boss->x, boss->y+1, 0, -1, 0);
        fireball->enemy = true;
        EntityTools::AppendEntity(data->entities, fireball);
        window->AddScreenShake(0.4f, 0);
        audio->PlaySound("Assets/Audio/FireballSling.wav");
    }

    if (boss->ShouldBridge()) {
        boss->BuildBridge(data->map);
        window->AddScreenShake(0, 0.7f); // Boss was hit, give satisfaction
    }

    if (boss->ShouldSmash() || boss->GetHealth() <= 0) {
        boss->Smash(data->map, data->entities);
        window->AddScreenShake(0.9f, 0); // Boss slammed ground
        Particle* smash = ActivateParticle(data->particles, ParticleID::EP_GolemSmash, boss->x, boss->y);
        smash->maxLifetime = 0.7f;
    }

    // Open doors
    if (boss->GetHealth() <= 0) {
        window->SetDialogueText("A sigh of relief...", 0);
        data->map->FillRectangle(0, 11, 1, 14, TileID::ET_Fizzler);
        data->map->FillRectangle(39, 11, 40, 14, TileID::ET_Fizzler);
    }
}