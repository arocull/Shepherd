#include "Map/map.h"

Map::Map() {
    // Initialize each tile in the map
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            tiles[x][y] = MakeTile(x, y, 0);
        }
    }

    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        StoredEntities[i] = nullptr;
    }
}

// Returns the tile ID of the given tile at X, Y
int Map::GetTileID(int x, int y) {
    return tiles[x][y]->id;
}
// Checks if the tile at X, Y is solid
bool Map::IsTileSolid(int x, int y) {
    return tiles[x][y]->solid;
}
// Checks if the tile at X, Y is a liquid
bool Map::IsTileLiquid(int x, int y) {
    return tiles[x][y]->liquid;
}



// Create walls within a rectangle X,Y sized centered within the map
void Map::WallRectangle(int x, int y) {
    int startX = (MapWidth-x)/2;
    int startY = (MapHeight-y)/2;
    int endX = startX + x;
    int endY = startY + y;

    WallRectangle(startX, startY, endX, endY, 1);
}
// Wall a set rectangle with whatever Tile you choose
void Map::WallRectangle(int startX, int startY, int endX, int endY, int id) {
    endX--;
    endY--;
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            if (((x == startX || x == endX) && (y >= startY && y <= endY)) || ((y == startY || y == endY) && (x >= startX && x <= endX)))
                SetTileID(tiles[x][y], id);
        }
    }
}
// Fill a set rectangle with whatever Tile you choose
void Map::FillRectangle(int startX, int startY, int endX, int endY, int id) {
    endX--;
    endY--;
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            if (x >= startX && x <= endX && y >= startY && y <= endY)
                SetTileID(tiles[x][y], id);
        }
    }
}

// Deallocates all tiles from RAM
void Map::Free() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            free(tiles[x][y]);
        }
    }
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (StoredEntities[i])
            delete StoredEntities[i];
    }
}