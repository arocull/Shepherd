#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "Core/config.h"
#include "Core/strutils.h"

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

        struct Puzzle* Puzzles;                // Puzzles that might accessible in levels.
        bool PuzzleStatus = false;      // Has the puzzle on this level been completed?

        bool ScrollDiscovered = false;  // Has the scroll on this map been read before?

        void SetMapID(int Identification);
        int GetMapID();
        void SetMapBiome(EnvironmentID b);
        EnvironmentID GetMapBiome();

        void SetScroll(const char* text, const char* name);
        bool HasScroll();
        char* GetScroll();
        char* GetScrollName();
        void SetScrollIndex(int newScrollIndex);
        int GetScrollIndex();

        int GetEventTimer();
        int GetEventID();
        bool TickEventTimer();
        void SetEventTimer(int time, int id = 0);

        void SetTile(int x, int y, int newID);
        int GetTileID(int x, int y);
        int GetTileIDConstrained(int x, int y);
        bool IsTileSolid(int x, int y);
        bool IsTileLiquid(int x, int y);
        bool IsTilePitfall(int x, int y);

        void WallRectangle(int x, int y);
        void WallRectangle(int startX, int startY, int endX, int endY, int id);
        void FillRectangle(int startX, int startY, int endX, int endY, int id);
        void FreezeArea(int centerX, int centerY, int radius, bool thaw);
        void Free();

        std::string* Ascii();

    private:
        int EntitiesStored = 0;
        int id = -1;                // Map ID, can be anything--repeats are fine. This is read by triggers to tell what level is what.
        EnvironmentID biome = EnvironmentID::ENV_Forest;           // What color should we render the background as?

        bool scrollSet = false;       // Has a scroll been set for this map?
        char* scroll = NULL;          // Scroll text for this area
        char* scrollName = NULL;      // Scroll name for this area
        int scrollIndex = -1;         // Index of the scroll in the collectibles list

        int eventTimer = -1;          // Counts down to a timed trigger event
        int eventID = 0;              // Use this to keep track of different timed events within the same level
};