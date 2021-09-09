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

    // Default all settings to none
    tile->liquid = false;
    tile->solid = false;
    tile->pitfall = false;
    tile->slidable = false;

    switch (newID) {
        case TileID::ET_Empty_Tile:    // Empty Tile
            tile->pitfall = true;
            break;
        case TileID::ET_Wall:     //Wall
        case TileID::ET_Tree:     //Tree
        case TileID::ET_Rock:     //Rock
        case TileID::ET_Pillar:   //Pillar
        case TileID::ET_Door_Vertical:    //Door (Closed Vertical)
        case TileID::ET_Door_Horizontal:  //Door (Closed Horizontal)
            tile->solid = true;
            break;
        case TileID::ET_Water:     //Water
        case TileID::ET_Magma:     //Magma
            tile->liquid = true;
            tile->slidable = true; // Boxes can be pushed into fluids
            break;
        case TileID::ET_Empty_Puzzle_Piece:
        case TileID::ET_Pressure_Plate:
            tile->slidable = true;
            break;
        default: // Default to nothing
            break;
    }
}


bool IsTileable(int tileID) {
    return tileID == TileID::ET_Vines;
}
int GetTilingIndex(bool up, bool down, bool right, bool left) {
    if (up && down && right && left) return 5;  // 4-way junction

    if (up && down) {
        if (right) return 14;
        if (left) return 15;
        return 6;       // Vertical
    }
    if (left && right) {
        if (down) return 12;
        if (up) return 13;
        return 7;    // Horizontal
    }

    if (down && right) return 8;
    if (down && left) return 9;
    if (up && right) return 10;
    if (up && left) return 11;

    if (up) return 2;       // Bottom cap
    if (down) return 1;     // Top cap
    if (right) return 4;    // Left cap
    if (left) return 3;     // Right cap

    return 0;
}