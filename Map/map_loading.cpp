// Level Loading //
#include "Map/map_loading.h"

bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos) {
    return (!level->IsTileSolid(xPos, yPos) && !level->IsTileLiquid(xPos, yPos) && !GetEntityAtLocation(entities, xPos, yPos));
}


Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Map* currentMap, Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY) {
    //Point current level to the new map
    Map* newLevel = world[worldX][worldY];

    // Dump any archivable entities into the map data (i.e. wolves)
    // Free current entity array and clean it (delete everything but the Shepherd)
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            if (currentMap && levelEntities[i]->archivable) {
                levelEntities[i]->Unload();
                AppendEntityDetailed(currentMap->StoredEntities, levelEntities[i], MaxEntitiesStoreable, levelEntities[i]->GetID());
            } else
                delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }
    if (currentMap)
        CleanEntities(currentMap->StoredEntities);
    CleanEntities(levelEntities);

    //Load entities from map into the new level entities
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (newLevel->StoredEntities[i]) {
            AppendEntity(levelEntities, newLevel->StoredEntities[i]);
            newLevel->StoredEntities[i] = nullptr;
        }
    }
    CleanEntities(newLevel->StoredEntities);


    //Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    if (DEBUG_RequireSheep && playerX >= 0 && playerY >= 0) {
        int sheepLeft = MaxSheep;

        int top = playerY-1; int bottom = playerY+1; int left = playerX-1; int right = playerX+1;
        
        // Summon sheep until all are summoned or no more available spawn points (NOTE: game will softlock if it runs out of spawn tiles)
        while (sheepLeft > 0 || (bottom == MapHeight && left == -1 && right == MapWidth && top == -1)) {
            top = max(0, top);
            left = max(0, left);
            bottom = min(MapHeight-1, bottom);
            right = min(MapWidth-1, right);

            for (int y = top; y <= bottom; y++) {
                for (int x = left; x <= right; x++) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, x, y) && sheepLeft > 0 && !(x == playerX && y == playerY)) {
                        AppendEntity(levelEntities, new Sheep(x, y));
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
Takes a map file and reads each character, generating a tilemap and entities based off of characters

Possibly in future: read a PNG file and convert it to map data.
*/
Map* GenerateMapFromFile(const char* filePath) {
    std::fstream mapFile;       //Creates stream to read from
    mapFile.open(filePath);     //Opens file path

    Map* map = new Map();       //Generates new map
    
    // Iterate through the map tiles, row by row 
    for (int y = 0; y < MapHeight && !mapFile.eof(); y++) {
        for (int x = 0; x < MapWidth && !mapFile.eof(); x++) {
            int charID;
            int tileID = 0;

            charID = mapFile.get(); //Get next character in line

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();
            

            bool SpawnsEntity = false;


            // Check character and set tile ID accordingly; defaults to 0 if none of these are present
            switch (charID) {
                case 'W': tileID = 1; break;    //Wall
                case 'w': tileID = 2; break;    //Water
                case 'L': tileID = 3; break;    //Lava
                case 'T': tileID = 4; break;    //Tree
                case 'R': tileID = 5; break;    //Rock
                case 'P': tileID = 6; break;    //Pillar
                case 'e': tileID = 7; break;    //Empty Puzzle Peace
                case 'p': tileID = 8; break;    //Pressure Plate
                case 'F': tileID = 9; break;    //Fake Wall

                // Map Triggers
                case '1': tileID = -1; break;   //Trigger 1
                case '2': tileID = -2; break;   //Trigger 2
                case '3': tileID = -3; break;   //Trigger 3
                case '4': tileID = -4; break;   //Trigger 4 - Note: no debounce

                // Entity Spawns - Any character IDs that are not used for tiles are marked as entity-spawning and redirected
                case 'h':
                    SpawnsEntity = true;
            }
            if (SpawnsEntity) {
                Entity* entity;
                if (charID == 'h')
                    entity = new Wolf(x, y);

                if (entity) {
                    entity->archivable = true;
                    AppendEntity(map->StoredEntities, entity);
                }
            }
            
            SetTileID(&(map->tiles[x][y]), tileID);
        }
    }

    mapFile.get();  // Dump last newline to get to map data
    if (!mapFile.eof()) {
        char digit1 = mapFile.get();
        char digit2 = mapFile.get();

        int mapID = ((int) (digit1 - '0')) * 10 + ((int) (digit2 - '0'));
        map->SetMapID(mapID);

        char biome = mapFile.get();
        if (!mapFile.eof())
            map->SetMapBiome(biome);
    }

    mapFile.close();

    return map;
}