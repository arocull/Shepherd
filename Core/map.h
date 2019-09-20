#include <Tile/tile.h>
#include "config.h"

class Map {
    public:
        Map();

        Tile* tiles[MapWidth][MapHeight];

        void WallRectangle(int x, int y);
        void Free();
};