#pragma once

#include "Core/config.h"
#include "Core/enums.h"

#include "Map/map.h"
#include "Entities/entity.h"
#include "Entities/particle.h"
#include "Entities/Subclasses/shepherd.h"

class GameData {
    public:
        GameData();
        ~GameData();

        Map*** world; // World where all map data is contained
        Shepherd* player; // Player character
        
        Map* map; // Currently loaded map that player and entities can interact with
        Entity** entities; // Current entities that are on-screen
        struct Particle* particles; // Existing particle instances
        
        int worldX; // Current X position in world
        int worldY; // Current Y position in world

        int ticks; // Total number of ticks the game has been running for

    private:

    public:
        int CountSheep();
};