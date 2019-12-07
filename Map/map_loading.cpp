// Level Loading //
#include "Map/map_loading.h"

bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos) {
    return (!level->IsTileSolid(xPos, yPos) && !level->IsTileLiquid(xPos, yPos) && !GetEntityAtLocation(entities, xPos, yPos));
}


Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY) {
    //Dump entities in current level back into the map data (exclude sheep, fireballs)


    //Point current level to the new map
    Map* newLevel = world[worldX][worldY];

    // Frees current entity array and cleans it (ignores Shepherds)
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }
    CleanEntities(levelEntities);

    //Load entities from map into the new level entities
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (newLevel->StoredEntities[i]) {
            AppendEntity(levelEntities, newLevel->StoredEntities[i]);
            newLevel->StoredEntities[i] = nullptr;
        }
    }
    CleanEntities(newLevel->StoredEntities);
    CleanEntities(levelEntities);


    //Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    if (DEBUG_RequireSheep && playerX >= 0 && playerY >= 0) {
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

    // Job's done!
    return newLevel;
}


/*
Currently: Takes a map file and reads each character, generating a tilemap based off char

Possibly in future: read a PNG file and convert it to map data.

References for future:
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
    //printf("Generating map from file %s...\n", filePath);
    
    // Iterate through the map tiles, row by row 
    for (int y = 0; y < MapHeight && !mapFile.eof(); y++) {
        for (int x = 0; x < MapWidth && !mapFile.eof(); x++) {
            int charID;
            int tileID = 0;

            charID = mapFile.get(); //Get next character in line

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();
            

            // Check character and set tile ID accordingly; defaults to 0 if none of these are present
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
            else if (charID == 'h') { // ENTITY - Wolf (H for hound)
                Wolf* enemy = new Wolf(x, y);
                enemy->archivable = true;
                AppendEntity(map->StoredEntities, enemy);
            }
            
            SetTileID(map->tiles[x][y], tileID); 
        }
    }

    mapFile.close();

    return map;
}