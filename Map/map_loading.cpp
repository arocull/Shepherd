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
            int tileID = 0;

            charID = mapFile.get(); //Get next character in line

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();
            

            bool SpawnsEntity = false;


            // Check character and set tile ID accordingly; defaults to 0 if none of these are present
            switch (charID) {
                case 'W': tileID = TileID::ET_Wall; break;    //Wall
                case 'w': tileID = TileID::ET_Water; break;    //Water
                case 'L': tileID = TileID::ET_Magma; break;    //Lava
                case 'T': tileID = TileID::ET_Tree; break;    //Tree
                case 'R': tileID = TileID::ET_Rock; break;    //Rock
                case 'P': tileID = TileID::ET_Pillar; break;    //Pillar
                case 'e': tileID = TileID::ET_Empty_Puzzle_Piece; break;    //Empty Puzzle Piece
                case 'p': tileID = TileID::ET_Pressure_Plate; break;    //Pressure Plate
                case 'F': tileID = TileID::ET_Fake_Wall; break;    //Fake Wall
                case 'E': tileID = TileID::ET_Empty_Tile; break;   //Empty Tile (black)
                case 'D': tileID = TileID::ET_Door_Vertical; break;  //Door (Closed Vertical)
                case 'd': tileID = TileID::ET_Door_Horizontal; break;  //Door (Closed Horizontal)
                case 'i': tileID = TileID::ET_Ice; break;  //Ice
                case 'v': tileID = TileID::ET_Vines; break;  // Vines
                case 's': tileID = TileID::ET_Scroll; break; // Scroll, a unique triggerable
                case 'I': tileID = TileID::ET_Indicator; break; // Indicator
                

                // Map Triggers
                case '1': tileID = TileID::ET_Trigger1; break;   //Trigger 1
                case '2': tileID = TileID::ET_Trigger2; break;   //Trigger 2
                case '3': tileID = TileID::ET_Trigger3; break;   //Trigger 3
                case '4': tileID = TileID::ET_Trigger4; break;   //Trigger 4 - Note: no debounce
                case 'f': tileID = TileID::ET_Fizzler; break;    //Fizzler - Extinguishes staff flames

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
                    SpawnsEntity = true;
            }
            if (SpawnsEntity) {
                Entity* entity;
                if (charID == 'h')
                    entity = new Wolf(x, y);
                else if (charID == 'c' || charID == 'C')
                    entity = new Crate(x, y);
                else if (charID == 't')
                    entity = new Torch(x, y);
                else if (charID == 'l')
                    entity = new Lever(x, y);

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
            map->SetMapBiome(biome);

            // After biomes, the only thing left in map data is scroll text, so build a string and set it using that
            mapFile.get();  // Clear out newline
            if (!mapFile.eof()) {   // Make sure we aren't at the end of the file though
                char* scrollName = (char*) calloc(sizeof(char), MaxScrollNameLength);

                int scrollIndex = 0;
                for (; scrollIndex < MaxScrollNameLength - 1 && !mapFile.eof(); scrollIndex++) {
                    char currentChar = mapFile.get();
                    if (currentChar == '\n') break; // Stop sampling on newline
                    scrollName[scrollIndex] = currentChar;
                }
                scrollName[scrollIndex - 1] = '\0';     // Terminating character

                // Do not need to grab newline here--is collected within scroll name builder
                if (!mapFile.eof()) {
                    char* scroll = (char*) calloc(sizeof(char), MaxScrollLength);
                    scrollIndex = 0; // Reset scroll index
                    for (; scrollIndex < MaxScrollLength - 1 && !mapFile.eof(); scrollIndex++) {
                        scroll[scrollIndex] = mapFile.get();
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