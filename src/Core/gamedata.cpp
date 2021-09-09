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