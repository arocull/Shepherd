#include "Map/tile.h"

// Creates a tile object at position X, Y and automatically sets its properties based off of ID
struct Tile* MakeTile(int x, int y, int id) {
    // Force-clear the spot of memory to avoid tile corruption
    struct Tile* tile = (struct Tile*) calloc(1, sizeof(struct Tile));
    tile->x = x;
    tile->y = y;
    SetTileID(tile, id);
    return tile;
}

// Sets tile's ID and properties based off of the given ID
void SetTileID(struct Tile* tile, int newID) {
    tile->id = newID;
    switch (newID) {
        case 10:    //Empty Tile
            tile->solid = false;
            tile->pitfall = true;
            break;
        case 1:     //Wall
        case 4:     //Tree
        case 5:     //Rock
        case 6:     //Pillar
        case 11:    //Door (Closed Vertical)
        case 12:    //Door (Closed Horizontal)
            tile->solid = true;
            break;
        case 2:     //Water
        case 3:     //Magma
            tile->liquid = true;
            tile->solid = false;
            tile->pitfall = false;
            break;
        case 13:    //Ice
        default:
            tile->solid = false;
            tile->liquid = false;
            tile->pitfall = false;
    }
}