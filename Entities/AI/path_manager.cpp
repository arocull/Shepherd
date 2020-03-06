#include "Entities/AI/path_manager.h"

/*
bool Movement_CheckCollision(Map* world, Entity* entities[MaxEntities], int x, int y) {
    return !(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        world->IsTileSolid(x, y) ||
        (GetEntityAtLocation(entities, x, y) && GetEntityAtLocation(entities, x, y)->Solid)
    );
}
*/

Tile** GetWalkableTiles(int posX, int posY, Map* map, Entity** entities, bool avoidLiquids) {
    Tile** tiles = (Tile**) malloc(4*sizeof(Tile*));    // Quickly allocate memory of tiles

    for (int x = posX - 1; x <= posX + 1; x++) {
        for (int y = posY - 1; y <= posY + 1; y++) {
            if (
                x >= 0 && x < MapWidth && y >= 0 && y < MapHeight
            )         
                printf("Tile pathable"); 
        }
    }
}

Path* GetFastestPath(int startX, int startY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids) {
    
}