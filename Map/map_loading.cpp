// Level Loading //
#include "Map/map_loading.h"

bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos) {
    return (!level->tiles[xPos][yPos]->IsSolid() && !level->tiles[xPos][yPos]->IsLiquid() && !GetEntityAtLocation(entities, xPos, yPos));
}


Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY) {
    //Dump entities in current level back into the map data (exclude sheep, fireballs)

    Map* newLevel = world[worldX][worldY];  //Point current level to the new map

    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }

    CleanEntities(levelEntities);

    //Load entities from map into the new level entities

    //Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    if (playerX >= 0 && playerY >= 0) {
        int sheepLeft = MaxSheep;

        int top = playerY-1; int bottom = playerY+1; int left = playerX-1; int right = playerX+1;
        int dir = 0;    //0 = right, 1 = down, 2 = left, 3 = up

        // Summon sheep until all are summoned or no more available spawn points (NOTE: game will softlock if it runs out of spawn tiles)
        while (sheepLeft > 0 || (bottom == MapHeight && left == -1 && right == MapWidth && top == -1)) {
            top = max(0, top);
            left = max(0, left);
            bottom = min(MapHeight-1, bottom);
            right = min(MapWidth-1, right);

            for (int y = top; y <= bottom; y++) {
                for (int x = left; x <= right; x++) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, x, y) && sheepLeft > 0 && !(x == playerX && y == playerY)) {
                        AppendEntity(levelEntities, new Entity(x, y, 2));
                        sheepLeft--;
                    }
                }
            }

            bottom++;
            top--;
            left--;
            right++;
        }
    }


    return newLevel;
}


/*
We want to read a PNG file and convert it to map data.


https://stackoverflow.com/questions/15510507/open-a-png-file-and-read-its-hex-values-in-c

https://www.zarb.org/~gc/html/libpng.html
*/
Map* GenerateMapFromPNG() {


    return nullptr;
}