#include "map.h"


Map::Map() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            tiles[x][y] = new Tile(x, y, 0);
        }
    }
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
                tiles[x][y]->SetTileID(id);
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
                tiles[x][y]->SetTileID(id);
        }
    }
}

// Deallocates all tiles from RAM
void Map::Free() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            delete tiles[x][y];
        }
    }
}