// Level Loading //
#include "Map/map_loading.h"

bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos) {
    return (!level->IsTileSolid(xPos, yPos) && !level->IsTileLiquid(xPos, yPos) && !level->IsTilePitfall(xPos, yPos) && !GetEntityAtLocation(entities, xPos, yPos));
}


Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Map* currentMap, Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY) {
    //Point current level to the new map
    Map* newLevel = world[worldX][worldY];

    // Dump any archivable entities into the map data (i.e. wolves)
    // Free current entity array and clean it (delete everything but the Shepherd)
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            levelEntities[i]->Unload();
            if (currentMap && levelEntities[i]->archivable) {
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


    // Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    if (
        playerX >= 0 && playerY >= 0
        #ifdef DEBUG_MODE
            && DEBUG_RequireSheep
        #endif
    ) {
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
            int tileID = TileID::ET_None;

            charID = mapFile.get(); //Get next character in line

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();
            

            bool SpawnsEntity = false;


            // Check character and set tile ID accordingly; defaults to 0 if none of these are present
            switch (charID) {
                // Entity Spawns - Any character IDs that are not used for tiles are marked as entity-spawning and redirected
                case 'c':       // Crate (with Empty Puzzle Piece tile beneath)
                    tileID = TileID::ET_Empty_Puzzle_Piece;
                    SpawnsEntity = true;
                    break;
                case 'C':       // Crate (with Pressure Plate tile beneath)
                    tileID = TileID::ET_Pressure_Plate;
                    SpawnsEntity = true;
                    break;
                case 'h':       // Wolf (or Hound)
                case 't':       // Torch
                case 'l':       // Lever
                case 'S':       // Spirit
                    SpawnsEntity = true;
                    break;

                default:
                    tileID = charID;
                    break;
            }
            if (SpawnsEntity) {
                Entity* entity = nullptr;
                if (charID == 'h')
                    entity = new Wolf(x, y);
                else if (charID == 'c' || charID == 'C')
                    entity = new Crate(x, y);
                else if (charID == 't')
                    entity = new Torch(x, y);
                else if (charID == 'l')
                    entity = new Lever(x, y);
                else if (charID == 'S')
                    entity = new Spirit(x, y);

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

        int mapID = ((int) (digit1 - '0')) * 10 + ((int) (digit2 - '0')); // Convert map ID to number
        map->SetMapID(mapID);

        char biome = mapFile.get();
        if (!mapFile.eof()) { // If there is a biome, get it
            map->SetMapBiome((EnvironmentID) biome); // Cast char to EnvironmentID (for standardization purposes/easier to keep track of)

            // After biomes, the only thing left in map data is scroll text, so build a string and set it using that
            mapFile.get();  // Clear out newline
            if (!mapFile.eof()) {   // Make sure we aren't at the end of the file though
                char* scrollName = (char*) calloc(sizeof(char), MaxScrollNameLength);

                int scrollIndex = 0;
                for (; scrollIndex < MaxScrollNameLength - 1 && !mapFile.eof(); scrollIndex++) {
                    char currentChar = mapFile.get();
                    scrollName[scrollIndex] = currentChar;
                    if (currentChar == '\n') { // Stop sampling on newline, character will get replaced with a terminating character
                        scrollIndex++;
                        break;
                    }
                }
                scrollName[scrollIndex - 1] = '\0';     // Terminating character
                scrollIndex = 0; // Reset scroll text index

                // Do not need to grab newline here--is collected within scroll name builder
                if (!mapFile.eof()) {
                    char* scroll = (char*) calloc(sizeof(char), MaxScrollLength);
                    for (; scrollIndex < MaxScrollLength - 1 && !mapFile.eof(); scrollIndex++) {
                        char currentChar = mapFile.get();
                        scroll[scrollIndex] = currentChar;
                        if (currentChar == '\n') { // Stop sampling on newline, character will get replaced with a terminating character
                            scrollIndex++;
                            break;
                        }
                    }
                    scroll[scrollIndex - 1] = '\0';     // Terminating character

                    map->SetScroll(scroll, scrollName);
                    free(scroll);
                }
                free(scrollName);
            }
        }
    }

    mapFile.close();

    return map;
}