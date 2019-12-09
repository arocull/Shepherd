#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "Map/tile.h"
#include "Entities/entity.h"

// Map - A class that holds a 2D array of tiles, as well as a list of entities that persist only within the map.
class Map {
    public:
        Map();

        struct Tile** tiles;
        Entity** StoredEntities;
        bool Triggers[3];

        void SetMapID(int Identification);
        int GetMapID();

        int GetTileID(int x, int y);
        bool IsTileSolid(int x, int y);
        bool IsTileLiquid(int x, int y);

        void WallRectangle(int x, int y);
        void WallRectangle(int startX, int startY, int endX, int endY, int id);
        void FillRectangle(int startX, int startY, int endX, int endY, int id);
        void Free();

    private:
        int EntitiesStored = 0;
        int id = -1;
};