#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "Core/enums.h"

struct Tile {
    int x;
    int y;
    int id = 0;
    bool solid = false;
    bool liquid = false;
    bool pitfall = false;
};

void SetTileID(struct Tile* tile, int newID);
struct Tile* MakeTile(int x, int y, int id);


bool IsTileable(int tileID);
int GetTilingIndex(bool up, bool down, bool right, bool left);