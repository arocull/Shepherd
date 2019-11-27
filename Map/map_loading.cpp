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

https://stackoverflow.com/questions/31079947/how-can-i-manually-read-png-files-in-c

http://paulbourke.net/dataformats/bmp/

https://www.youtube.com/watch?v=1Fo-5vJcB4w     //Using this
at 7 minutes
*/
Map* GenerateMapFromFile(const char* filePath) {
    std::fstream mapFile;       //Creates stream to read from
    mapFile.open(filePath);     //Opens file path

    Map* map = new Map();       //Generates new map
    
    for (int y = 0; y < MapHeight && !mapFile.eof(); y++) {
        for (int x = 0; x < MapWidth && !mapFile.eof(); x++) {
            int charID;
            int tileID = 0;

            charID = mapFile.get();

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();
            

            if (charID == 'W')      //Wall
                tileID = 1;
            else if (charID == 'w') //Water
                tileID = 2;
            else if (charID == 'L') //Lava
                tileID = 3;
            else if (charID == 'T') //Tree
                tileID = 4;
            else if (charID == 'R') //Rock
                tileID = 5;
            
            map->tiles[x][y]->SetTileID(tileID); 
        }
    }

    mapFile.close();

    return map;
}