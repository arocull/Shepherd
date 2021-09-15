#pragma once

#include "Core/config.h"
#include "Core/enums.h"
#include "Core/mathutil.h"

#include "Core/gamedata.h"
#include "Core/renderwindow.h"
#include "Audio/sound_service.h"
#include "Entities/entity.h"

#include "Entities/Subclasses/sheep.h"
#include "Entities/Subclasses/wolf.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/Boss/pyramidgolem.h"

#include "Map/map.h"
#include "Entities/movement.h"
#include "Entities/AI/path_manager.h"

// AI Manager - Takes a context and makes decisions for given entities, including movement, attacks, and interactions
class AIManager {
    public:
        AIManager(GameData* data, RenderWindow* window, SoundService* audio);
    
    private:
        GameData* data;
        RenderWindow* window;
        SoundService* audio;
        Entity* enemyGoal;

        void TickSheep(Sheep* sheep);
        void TickWolf(Wolf* wolf);
        void TickFireball(Fireball* fireball);

        void TickPyramidGolem(PyramidGolem* boss);

    public:
        void UpdateContext();
        void TickAI(Entity* entity);
};