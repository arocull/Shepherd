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

    for (int x = 0; x < WorldWidth; x++) {
        free(world[x]);
    }
    free(world);

    free(entities);
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