#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "Map/tile.h"
#include "Entities/entity.h"
#include "Map/puzzle.h"

// Map - A class that holds a 2D array of tiles, as well as a list of entities that persist only within the map.
class Map {
    public:
        Map();

        struct Tile** tiles;            // 2D Array of map tiles
        Entity** StoredEntities;        // Unloaded entities stored within the map data
        bool Triggers[3];               // Booleans for trigger variables
        int PressurePlatesPressed = 0;  // The number of pressure plates currently weighed down at the current frame
        bool HasLoaded = false;         // Is set to true once a map has been physically loaded

        Puzzle* Puzzles;                // Puzzles that might accessible in levels.
        bool PuzzleStatus = false;      // Has the puzzle on this level been completed?

        void SetMapID(int Identification);
        int GetMapID();
        void SetMapBiome(char b);
        char GetMapBiome();

        void SetTile(int x, int y, int newID);
        int GetTileID(int x, int y);
        bool IsTileSolid(int x, int y);
        bool IsTileLiquid(int x, int y);

        void WallRectangle(int x, int y);
        void WallRectangle(int startX, int startY, int endX, int endY, int id);
        void FillRectangle(int startX, int startY, int endX, int endY, int id);
        void FreezeArea(int centerX, int centerY, int radius, bool thaw);
        void Free();

    private:
        int EntitiesStored = 0;
        int id = -1;                // Map ID, can be anything--repeats are fine. This is read by triggers to tell what level is what.
        char biome = 'F';           // What color should we render the background as?
};