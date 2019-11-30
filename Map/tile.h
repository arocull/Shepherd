#pragma once

#include <stdlib.h>
#include <stdbool.h>

struct Tile {
    int x;
    int y;
    int id = 0;
    bool solid = false;
    bool liquid = false;
};

void SetTileID(struct Tile* tile, int newID);
struct Tile* MakeTile(int x, int y, int id);