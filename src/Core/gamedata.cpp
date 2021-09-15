#include "Core/gamedata.h"

GameData::GameData() {
    world = (Map***) calloc(WorldWidth, sizeof(Map**));
    for (int x = 0; x < WorldWidth; x++) {
        world[x] = (Map**) calloc(WorldHeight, sizeof(Map*));
    }

    entities = (Entity**) calloc(MaxEntities, sizeof(Entity*));
}
GameData::~GameData() {
    StopParticles(particles);
    free(particles);

    // Delete all entities that are not the player
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->GetID() != EntityID::EE_Shepherd) {
            entities[i]->Unload();
            delete entities[i];
        }
    }
    free(entities);
    delete player; // Delete player

    // Delete loaded maps
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y]->Free();
            delete world[x][y];
        }
    }

    // Delete world
    for (int x = 0; x < WorldWidth; x++) {
        free(world[x]);
    }
    free(world);
}


int GameData::CountSheep() {
    int sheep = 0;
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->GetID() == EntityID::EE_Sheep) {
            sheep++;
        }
    }
    return sheep;
}