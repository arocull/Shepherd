#include "Map/map.h"

Map::Map() {
    // Initialize each tile in the map
    tiles = (struct Tile**) calloc(MapWidth, sizeof(struct Tile*));
    for (int x = 0; x < MapWidth; x++) {
        tiles[x] = (struct Tile*) calloc(MapHeight, sizeof(struct Tile));
        for (int y = 0; y < MapHeight; y++) {
            tiles[x][y].x = x;
            tiles[x][y].y = y;
            SetTileID(&tiles[x][y], 0);
        }
    }

    StoredEntities = (Entity**) calloc(MaxEntitiesStoreable, sizeof(Entity));
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        StoredEntities[i] = nullptr;
    }

    for (int i = 0; i < 3; i++)
        Triggers[i] = false;
}


void Map::SetMapID(int Identification) {
    id = Identification;
}
int Map::GetMapID() {
    return id;
}
void Map::SetMapBiome(char b) {
    biome = b;
}
char Map::GetMapBiome() {
    return biome;
}


// Sets the tile properties of the tile at X, Y based off the given ID
void Map::SetTile(int x, int y, int newID) {
    SetTileID(&tiles[x][y], newID);
}
// Returns the tile ID of the given tile at X, Y
int Map::GetTileID(int x, int y) {
    return tiles[x][y].id;
}
// Checks if the tile at X, Y is solid
bool Map::IsTileSolid(int x, int y) {
    return tiles[x][y].solid;
}
// Checks if the tile at X, Y is a liquid
bool Map::IsTileLiquid(int x, int y) {
    return tiles[x][y].liquid;
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
                SetTileID(&tiles[x][y], id);
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
                SetTileID(&tiles[x][y], id);
        }
    }
}

// Deallocates all tiles from RAM
void Map::Free() {
    for (int x = 0; x < MapWidth; x++) {
        free(tiles[x]);
    }
    free(tiles);
    
    delete StoredEntities;
}