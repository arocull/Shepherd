#include "Entities/AI/path_manager.h"


bool IsTilePathable(int x, int y, Map* map, Entity** entities, bool avoidLiquids) {
    Entity* a = GetEntityAtLocation(entities, x, y);
    return (
        x >= 0 && x < MapWidth && y >= 0 && y < MapHeight &&    // Is tile within map bounds?
        (!map->IsTileSolid(x,y) && !(map->IsTileLiquid(x,y) && avoidLiquids) && !map->IsTilePitfall(x, y)) &&    // Check if tile is pathable
        !(a && a->Solid && a->GetID() != 1)    //Make sure tile is not already occupied, ignore Shepherd
    );
}



WalkableResults* Path_Internal_GetWalkableTiles(int posX, int posY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids) {
    WalkableResults* result = (WalkableResults*) malloc(sizeof(WalkableResults));
    result->tileX = (int*) malloc(4*sizeof(int));    // Quickly allocate memory of tiles
    result->tileY = (int*) malloc(4*sizeof(int));
    result->toB = (int*) malloc(4*sizeof(int));

    int current = 0;
    for (int x = posX - 1; x <= posX + 1; x++) {
        for (int y = posY - 1; y <= posY + 1; y++) {
            if ((x == posX || y == posY) && !(x == posX && y == posY)) {
                if (IsTilePathable(x, y, map, entities, avoidLiquids)) {
                    result->tileX[current] = x;
                    result->tileY[current] = y;
                    result->toB[current] = distGrid(x, y, goalX, goalY);
                } else {
                    result->tileX[current] = 0;
                    result->tileY[current] = 0;
                    result->toB[current] = 1000;
                }
                //printf("\t\tPATHABLE OPTION tile %i, %i with distance from goal %i\n", x, y, result->toB[current]);
                current++;
            }
        }
    }

    return result;
}
void Path_Internal_FreeWalkableResults(WalkableResults* toFree) {
    free(toFree->toB);
    free(toFree->tileY);
    free(toFree->tileX);
    free(toFree);
}


Path* GetPath(int startX, int startY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids) {
    Path* path = (Path*) calloc(1, sizeof(Path));
    path->tileX = (int*) malloc(MaxPathLength*sizeof(int));
    path->tileY = (int*) malloc(MaxPathLength*sizeof(int));

    path->goalX = goalX;
    path->goalY = goalY;

    // Clear malloc'd memory and set values
    path->tileX[0] = startX;
    path->tileY[0] = startY;
    for (int i = 1; i < MaxPathLength; i++) {
        path->tileX[i] = -1;
        path->tileY[i] = -1;
    }

    int currentX = startX;
    int currentY = startY;

    int maxDist = 1000;
    for (path->length = 1; path->length < MaxPathLength && !(currentX == goalX && currentY == goalY); path->length++) {
        WalkableResults* walk = Path_Internal_GetWalkableTiles(currentX, currentY, goalX, goalY, map, entities, avoidLiquids);

        // Find best point that is walkable that is closest to the goal and has not already been pathed
        for (int i = 0; i < 4; i++) {
            if (walk->toB[i] < maxDist) {

                bool pathed = false;    // Make sure the given tile has not been pathed upon
                for (int j = 0; j < path->length && !pathed; j++) {
                    if (walk->tileX[i] == path->tileX[j] && walk->tileY[i] == path->tileY[j])
                        pathed = true;
                }

                if (!pathed) {
                    maxDist = walk->toB[i];
                    currentX = walk->tileX[i];
                    currentY = walk->tileY[i];
                }
            }
        }

        Path_Internal_FreeWalkableResults(walk);

        if (maxDist == 1000) break;  // If no point was reachable, then the entity is trapped and pathing should be given up on

        path->tileX[path->length] = currentX;
        path->tileY[path->length] = currentY;
    }

    if (currentX == goalX && currentY == goalY) // If path reaches the endpoint, mark it as complete
        path->complete = true;
    else if (distGrid(currentX, currentY, goalX, goalY) >= distGrid(startX, startY, goalX, goalY))
        path->length = 0;   // If the path literally takes you further away from the endpoint, then do not bother following it

    return path;
}
void Path_Internal_CopyPathData(Path* from, Path* to) {
    to->length = from->length;
    to->complete = from->complete;
    to->goalX = from->goalX;
    to->goalY = from->goalY;
    for (int i = 0; i < from->length; i++) {
        to->tileX[i] = from->tileX[i];
        to->tileY[i] = from->tileY[i];
    }
}
void Path_FreePath(Path* toFree) {
    if (!toFree) return;
    free(toFree->tileY);
    free(toFree->tileX);
    free(toFree);
}




void CheckPathObscurity(Path* path, Map* map, Entity** entities, bool avoidLiquids) {
    if (!path || !path->complete) return;  // Return immeadietly if path does not exist or is already obscured/incomplete

    bool obscured = false;
    for (int i = 0; i < path->length && !obscured; i++) {
        if (!IsTilePathable(path->tileX[i], path->tileY[i], map, entities, avoidLiquids))
            obscured = true;
    }
    
    if (obscured)
        path->complete = false;
    
    return;
}

// Gets the next movement along a path
void GetNextMovement(int currentX, int currentY, Path* path, int* nextMoveX, int* nextMoveY) {
    if (!path) return;    // Return immeadietly if path does not exist

    for (int i = 0; i < path->length-1; i++) {
        if (currentX == path->tileX[i] && currentY == path->tileY[i]) {
            *nextMoveX = path->tileX[i+1] - currentX;
            *nextMoveY = path->tileY[i+1] - currentY;

            return;
        }
    }
}