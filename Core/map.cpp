#include "map.h"


Map::Map() {
    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            tiles[x][y]->SetPosition(x, y);
        }
    }
}


void Map::WallRectangle(int x, int y) {
    int startX = (MapWidth-x)/2-1;
    int startY = (MapHeight-y)/2-1;
    int endX = startX + x;
    int endY = startY + y;

    for (int x = 0; x < MapWidth; x++) {
        for (int y = 0; y < MapHeight; y++) {
            if (x == startX || x == endX || y == startY || y == endY)
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