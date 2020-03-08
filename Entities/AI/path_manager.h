// Module that finds the fastest, best possible path from point A to point B
// Created using this tutorial https://www.raywenderlich.com/3016-introduction-to-a-pathfinding
// Note - Currently this does not use A* pathfinding, but in the future I might rework it to allow so
#pragma once

#include "Core/config.h"

#include <stdbool.h>

#include "Core/mathutil.h"

#include "Map/map.h"
#include "Map/tile.h"

#include "Entities/entity.h"
#include "Entities/entity_management.h"

// A structure containing info on a set path of tiles 
struct Path {
    // Length of path in tiles
    int length;

    // Goal position of this path
    int goalX;
    int goalY;

    // Was the path able to be completed?
    bool complete;

    // List of tiles in order from closest to furthest on the path
    int* tileX;
    int* tileY;
};
// A structure containing info on data collected by a GetWalkableTiles call
struct WalkableResults {
    // List of tiles in order from closest to furthest on the path
    int* tileX;
    int* tileY;
    // A guess as to how close to point B the given tile is
    int* toB;
};

bool IsTilePathable(int x, int y, Map* map, Entity** entities, bool avoidLiquids);

// Returns an array of tiles that can be walked on
WalkableResults* Path_Internal_GetWalkableTiles(int posX, int posY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids);
// Frees a WalkableResults struct
void Path_Internal_FreeWalkableResults(WalkableResults* toFree);

// Gets a simple path from point A to point B
Path* GetPath(int startX, int startY, int goalX, int goalY, Map* map, Entity** entities, bool avoidLiquids);
// Copies down path data from a given path into another one--used for making copies of paths
void Path_Internal_CopyPathData(Path* from, Path* to);
// Frees pathdata
void Path_FreePath(Path* toFree);

// Checks to see if the path was obscured (is path blocked by entity or tile?) -- if so, marks path as incomplete
void CheckPathObscurity(Path* path, Map* map, Entity** entities, bool avoidLiquids);

// Gets the next movement along a path
void GetNextMovement(int currentX, int currentY, Path* path, int* nextMoveX, int* nextMoveY);