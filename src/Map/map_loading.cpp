// Level Loading //
#include "Map/map_loading.h"

bool LoadLevel_IsSpawnable(Map* level, Entity** entities, int xPos, int yPos) {
    return (!level->IsTileSolid(xPos, yPos) && !level->IsTileLiquid(xPos, yPos) && !level->IsTilePitfall(xPos, yPos) && !EntityTools::GetEntityAtLocation(entities, xPos, yPos));
}


Map* LoadLevel(Map*** world, Map* currentMap, Entity** levelEntities, int worldX, int worldY, int playerX, int playerY) {
    //Point current level to the new map
    Map* newLevel = world[worldX][worldY];

    // Dump any archivable entities into the map data (i.e. wolves)
    // Free current entity array and clean it (delete everything but the Shepherd)
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            levelEntities[i]->Unload();
            if (currentMap && levelEntities[i]->archivable) {
                EntityTools::AppendEntityDetailed(currentMap->StoredEntities, levelEntities[i], MaxEntitiesStoreable, levelEntities[i]->GetID());
            } else
                delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }
    if (currentMap)
        EntityTools::CleanEntities(currentMap->StoredEntities);
    EntityTools::CleanEntities(levelEntities);

    //Load entities from map into the new level entities
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (newLevel->StoredEntities[i]) {
            EntityTools::AppendEntity(levelEntities, newLevel->StoredEntities[i]);
            newLevel->StoredEntities[i] = nullptr;
        }
    }
    EntityTools::CleanEntities(newLevel->StoredEntities);


    // Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    #ifndef DEBUG_NoSheepRequired
    if (playerX >= 0 && playerY >= 0) {
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
                        EntityTools::AppendEntity(levelEntities, new Sheep(x, y));
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
    #endif

    // Job's done!
    return newLevel;
}