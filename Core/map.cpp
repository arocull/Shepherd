#include "map.h"


Map::Map() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            tiles[x][y] = new Tile(x, y, 0);
        }
    }
}


void Map::WallRectangle(int x, int y) {
    int startX = (MapWidth-x)/2;
    int startY = (MapHeight-y)/2;
    int endX = startX + x - 1;
    int endY = startY + y - 1;

    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            if (((x == startX || x == endX) && (y >= startY && y <= endY)) || ((y == startY || y == endY) && (x >= startX && x <= endX)))
                tiles[x][y]->SetTileID(1);
        }
    }
}
void Map::Free() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            delete tiles[x][y];
        }
    }
}