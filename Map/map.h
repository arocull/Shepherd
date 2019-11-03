#pragma once

#include <Map/tile.h>
#include "config.h"

// Map - A class that holds a 2D array of tiles, as well as a list of entities that persist only within the map.
class Map {
    public:
        Map();

        Tile* tiles[MapWidth][MapHeight];

        void WallRectangle(int x, int y);
        void WallRectangle(int startX, int startY, int endX, int endY, int id);
        void FillRectangle(int startX, int startY, int endX, int endY, int id);
        void Free();
};