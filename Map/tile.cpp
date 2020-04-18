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
        case TileID::ET_Empty_Tile:    //Empty Tile
            tile->liquid = false;
            tile->solid = false;
            tile->pitfall = true;
            break;
        case TileID::ET_Wall:     //Wall
        case TileID::ET_Tree:     //Tree
        case TileID::ET_Rock:     //Rock
        case TileID::ET_Pillar:     //Pillar
        case TileID::ET_Door_Vertical:    //Door (Closed Vertical)
        case TileID::ET_Door_Horizontal:    //Door (Closed Horizontal)
            tile->liquid = false;
            tile->solid = true;
            tile->pitfall = false;
            break;
        case TileID::ET_Water:     //Water
        case TileID::ET_Magma:     //Magma
            tile->liquid = true;
            tile->solid = false;
            tile->pitfall = false;
            break;
        default:
            tile->solid = false;
            tile->liquid = false;
            tile->pitfall = false;
    }
}