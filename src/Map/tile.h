#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "Core/enums.h"

struct Tile {
    int x;
    int y;
    int id = 0;
    bool solid = false; // If true, normal entities cannot move onto this object
    bool liquid = false; // If true, this acts as a liquid and blocks most pathing AI, and/or interacts with fires
    bool pitfall = false; // If true, fireballs can fly over this, but it is non-pathable
    bool slidable = false; // If true, boxes can be pushed over this tile. Currently unused.
};

void SetTileID(struct Tile* tile, int newID);
struct Tile* MakeTile(int x, int y, int id);


bool IsTileable(int tileID);
int GetTilingIndex(bool up, bool down, bool right, bool left);