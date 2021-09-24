// Level Loading //
#include "Map/map_loading.h"

bool LevelManager::IsSpawnable(Map* level, Entity** entities, int xPos, int yPos) {
    return (!level->IsTileSolid(xPos, yPos) && !level->IsTileLiquid(xPos, yPos) && !level->IsTilePitfall(xPos, yPos) && !EntityTools::GetEntityAtLocation(entities, xPos, yPos));
}

void LevelManager::UnloadArchivable(Map* map, Entity** entities) {
    if (nullptr == map) {
        return;
    }

    // Dump any archivable entities into the map data (i.e. wolves, torches, etc)
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->archivable) {
            entities[i]->Unload();
            EntityTools::AppendEntityDetailed(map->StoredEntities, entities[i], MaxEntitiesStoreable, entities[i]->GetID());
            entities[i] = nullptr;
        }
    }

    EntityTools::CleanEntities(map->StoredEntities);
}
void LevelManager::LoadArchivable(Map* map, Entity** entities) {
    //Load entities from map into the new level entities
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (map->StoredEntities[i]) {
            EntityTools::AppendEntity(entities, map->StoredEntities[i]);
            map->StoredEntities[i] = nullptr;
        }
    }
    EntityTools::CleanEntities(map->StoredEntities);
}


Map* LevelManager::LoadLevel(GameData* data, bool spawnSheep) {
    LevelManager::UnloadArchivable(data->map, data->entities); // Unload current level entities
    // Free current entity array and clean it (delete everything but the Shepherd and Sheep)
    for (int i = 0; i < MaxEntities; i++) {
        if (data->entities[i] && !(data->entities[i]->GetID() == EntityID::EE_Shepherd || data->entities[i]->GetID() == EntityID::EE_Sheep)) {
            data->entities[i]->Unload();
            delete data->entities[i];
            data->entities[i] = nullptr;
        }
    }

    Map* newLevel = data->world[data->worldX][data->worldY]; //Point current level to the new map

    //Load entities from map into the new level entities
    LevelManager::LoadArchivable(newLevel, data->entities);
    EntityTools::CleanEntities(data->entities);

    // Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    #ifndef DEBUG_NoSheepRequired
    int playerX = data->player->x;
    int playerY = data->player->y;

    // Keep track of all sheep
    int totalSheep = data->CountSheep();
    if (spawnSheep) {
        totalSheep = MaxSheep;
    }
    Entity** sheep = (Entity**) calloc(totalSheep, sizeof(Entity*)); // Create an array to easily keep track of sheep in
    for (int i = 0; i < totalSheep; i++) { // Zero out sheep array
        sheep[i] = nullptr;
        if (spawnSheep) {
            EntityTools::AppendEntity(data->entities, new Sheep(0, 0), MaxEntities);
        }
    }
    for (int i = 0; i < MaxEntities; i++) { // Pull sheep from level into sheep array
        if (data->entities[i] && data->entities[i]->GetID() == EntityID::EE_Sheep) {
            EntityTools::AppendEntity(sheep, data->entities[i], totalSheep);
        }
    }

    if (playerX >= 0 && playerY >= 0) {
        int sheepLeft = totalSheep;

        int top = playerY-1; int bottom = playerY+1; int left = playerX-1; int right = playerX+1;
        
        // Summon sheep until all are summoned or no more available spawn points (NOTE: game will softlock if it runs out of spawn tiles)
        while (sheepLeft > 0 || (bottom == MapHeight && left == -1 && right == MapWidth && top == -1)) {
            top = max(0, top);
            left = max(0, left);
            bottom = min(MapHeight-1, bottom);
            right = min(MapWidth-1, right);

            // Spawn sheep inside bounding box
            for (int y = top; y <= bottom; y++) {
                for (int x = left; x <= right; x++) {
                    // If the tile is a valid place to place a sheep....
                    if (IsSpawnable(newLevel, data->entities, x, y) && sheepLeft > 0 && !(x == playerX && y == playerY)) {
                        Entity* she = sheep[sheepLeft - 1]; // Grab a sheep
                        she->x = x; // Teleport them there
                        she->y = y;
                        sheepLeft--; // Decrement sheep left
                    }
                }
            }

            // Expand bounding box outward by 1
            bottom++;
            top--;
            left--;
            right++;
        }
    }
    free(sheep); // Don't need to keep track of sheep anymore
    #endif

    // Job's done!
    return newLevel;
}