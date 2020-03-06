// Module that finds the fastest, best possible path from point A to point B
// Creating using this tutorial https://www.raywenderlich.com/3016-introduction-to-a-pathfinding
#include "Core/config.h"

#include <stdbool.h>

#include "Map/map.h"
#include "Map/tile.h"

#include "Entities/entity.h"
#include "Entities/entity_management.h"

struct Path {
    int length = 0;
    Tile** tiles;
};

// Returns an array of tiles that can be walked on
Tile** GetWalkableTiles(int posX, int posY, Map* map, Entity** entities, bool avoidLiquids);

// Gets fastest from point A to point B
Path* GetFastestPath(int startX, int startY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids);