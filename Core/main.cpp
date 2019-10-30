/*
Written by Alan O'Cull
To compile:
$make clean
$make

To Debug:
$make run
$make debug
*/


#include <SDL2/SDL.h>
#include <cstdlib>

#include "Core/renderwindow.h"
#include "Core/map.h"

#include "Entities/entity.h"
#include "Entities/shepherd.h"
#include "Entities/fireball.h"

#include "config.h"
#include "mathutil.h"




// Entity Management
// Adds the selected entity to the given list of entities
void AppendEntity(Entity* entities[MaxEntities], Entity* newEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (!entities[i]) {            //Empty slot in list. Go ahead and add them in!
            entities[i] = newEntity;
            return;
        }
    }
}
/* Adds the selected entity to the given list of entities
Detailed - Filters out null data and prevents doubling up on things, while also having a defined range and a set filter*/
void AppendEntityDetailed(Entity* entities[MaxEntities], Entity* newEntity, int dataRange, int filter) {
    if (!newEntity || newEntity->GetID() != filter) return;                 //Don't attempt to add something that doesn't exist!

    for (int i = 0; i < dataRange; i++) {
        if (entities[i] == newEntity)       //Entity already exists within list! Don't add them again!
            return;
        else if (!entities[i]) {            //Empty slot in list. Go ahead and add them in!
            entities[i] = newEntity;
            return;
        }
    }
}
// Removed the selected entity from the given list of entities
void RemoveEntity(Entity* entities[MaxEntities], Entity* delEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && delEntity && entities[i] == delEntity) {
            delete entities[i];
            entities[i] = nullptr;
            return;
        }
    }
}
// Prune the list of entities
void CleanEntities(Entity* entities[MaxEntities]) {
    for (int i = MaxEntities-1; i > 0; i--) {
        if (!entities[i-1] && entities[i]) {
            entities[i-1] = entities[i];
            entities[i] = nullptr;
        }
    }
}
Entity* GetEntityAtLocation(Entity* entities[MaxEntities], int xPos, int yPos) {
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->x == xPos && entities[i]->y == yPos)
            return entities[i];
    }

    return nullptr;
}
/* Checks to make sure all sheep are touching the shepherd (or touching other sheep touching the shepherd)
Warning - Has many iterations and can cause a lot of overhead. Use sparingly*/
bool HasAllSheep(Entity* entities[MaxEntities], Entity* obj) {
    Entity* touchedSheep[MaxSheep+1];
    for (int i = 1; i <= MaxSheep; i++) {   //Initialize list
        touchedSheep[i] = nullptr;
    }
    touchedSheep[0] = obj;

    int sheep = 0;
    
    for (int i = 0; i <= MaxSheep; i++) {
        if (touchedSheep[i]) {
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x+1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y+1),  MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x-1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y-1),  MaxSheep+1, 2);
            if (touchedSheep[i]->GetID() == 2)
                sheep++;
        }
    }

    //printf("Sheep tally is %i sheep\n", sheep);

    return sheep >= MaxSheep;
}




// Level Loading
bool LoadLevel_IsSpawnable(Map* level, Entity* entities[MaxEntities], int xPos, int yPos) {
    return (!level->tiles[xPos][yPos]->IsSolid() && !level->tiles[xPos][yPos]->IsLiquid() && !GetEntityAtLocation(entities, xPos, yPos));
}
Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Entity* levelEntities[MaxEntities], int worldX, int worldY, int playerX, int playerY) {
    //Dump entities in current level back into the map data (exclude sheep, fireballs)

    Map* newLevel = world[worldX][worldY];  //Point current level to the new map

    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i]) {
            delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }

    //Load entities from map into the new level entities

    //Spawn Sheep (if playerX or playerY is negative, do not spawn any)
    if (playerX >= 0 && playerY >= 0) {
        int sheepLeft = MaxSheep;

        int top = playerY+1; int bottom = playerY-1; int left = playerX-1; int right = playerX+1;
        int dir = 0;    //0 = right, 1 = down, 2 = left, 3 = up

        while (sheepLeft > 0) {
            bottom = max(0, bottom);
            left = max(0, left);
            top = min(MapHeight, top);
            right = min(MapWidth, right);

            if (dir == 0) {
                for (int x = left; x <= right && x < MapWidth && x >= 0; x++) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, x, top) && sheepLeft > 0) {
                        AppendEntity(levelEntities, new Entity(x, top, 2));
                        sheepLeft--;
                    }
                }
                top++;
                dir = 1;
            } else if (dir == 1) {
                for (int y = top; y >= bottom && y < MapHeight && y >= 0; y--) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, right, y) && sheepLeft > 0) {
                        AppendEntity(levelEntities, new Entity(right, y, 2));
                        sheepLeft--;
                    }
                }
                right++;
                dir = 2;
            } else if (dir == 2) {
                for (int x = right; x >= left && x < MapWidth && x >= 0; x--) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, x, bottom) && sheepLeft > 0) {
                        AppendEntity(levelEntities, new Entity(x, bottom, 2));
                        sheepLeft--;
                    }
                }
                bottom--;
                dir = 3;
            } else if (dir == 3) {
                for (int y = bottom; y <= top && y < MapHeight && y >= 0; y++) {
                    if (LoadLevel_IsSpawnable(newLevel, levelEntities, left, y) && sheepLeft > 0) {
                        AppendEntity(levelEntities, new Entity(left, y, 2));
                        sheepLeft--;
                    }
                }
                left--;
                dir = 0;
            }
        }
    }


    return newLevel;
}





// Movement //
bool MoveUp = false;
bool MoveDown = false;
bool MoveRight = false;
bool MoveLeft = false;
bool Move_QueueClear = false;
bool MoveFireballQueued = false;
void Movement_Clear() {
    MoveUp = false;
    MoveDown = false;
    MoveRight = false;
    MoveLeft = false;
    Move_QueueClear = false;
}
// Push entity from point A to point B
void Movement_ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    int distX = abs(dx);
    int distY = abs(dy);

    int xChange = sgn(dx);
    int yChange = -sgn(dy);

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    //A* pathing?
    
    if (distY > distX) {
        for (int stepY = 0; stepY < distY; stepY++) {
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->tiles[obj->x][obj->y+yChange]->IsSolid() || GetEntityAtLocation(entities, obj->x, obj->y+yChange))
                break;
            else
                obj->y+=yChange;
        }
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->tiles[obj->x+xChange][obj->y]->IsSolid() || GetEntityAtLocation(entities, obj->x+xChange, obj->y))
                break;
            else
                obj->x+=xChange;
        }
    } else {
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->tiles[obj->x+xChange][obj->y]->IsSolid() || GetEntityAtLocation(entities, obj->x+xChange, obj->y))
                break;
            else
                obj->x+=xChange;
        }
        for (int stepY = 0; stepY < distY; stepY++) {
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->tiles[obj->x][obj->y+yChange]->IsSolid() || GetEntityAtLocation(entities, obj->x, obj->y+yChange))
                break;
            else
                obj->y+=yChange;
        }
    }

    //SDL_Log("Attempting step %i, %i with signs %i, %i\tNew Position: %i, %i", dx, dy, distX, distY, obj->x, obj->y);

    return;
}
void Movement_ShiftPlayer(Map* world, Entity* entities[MaxEntities], Shepherd* obj, int dx, int dy, int* worldX, int* worldY) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    obj->lastX = dx;
    obj->lastY = dy;
    obj->animation = 1;     //Set animation to walking

    if (desiredX >= MapWidth && *worldX+1 <= WorldWidth-1 && HasAllSheep(entities, obj)) {
        *worldX = *worldX+1;
        obj->x = 0;
    } else if (desiredX < 0 && *worldX-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldX = *worldX-1;
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !world->tiles[desiredX][obj->y]->IsSolid()) {
        Entity* hit = GetEntityAtLocation(entities, desiredX, obj->y);
        if (hit && hit->GetID() == 2) {   //If shep, swap places
            hit->x = obj->x;
            obj->Flipped = !obj->Flipped;
        }

        obj->x = desiredX;
    }
    
    if (desiredY < 0 && *worldY+1 <= WorldHeight-1 && HasAllSheep(entities, obj)) {
        *worldY = *worldY+1;
        obj->y = MapHeight-1;
    } else if (desiredY >= MapHeight && *worldY-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldY = *worldY-1;
        obj->y = 0;
    } else if (desiredY < MapHeight && desiredY >= 0 && !world->tiles[obj->x][desiredY]->IsSolid()) {
        Entity* hit = GetEntityAtLocation(entities, obj->x, desiredY);
        if (hit && hit->GetID() == 2)   //If shep, swap places
            hit->y = obj->y;
            
        obj->y = desiredY;
    }
    
    return;
}









int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize program: %s", SDL_GetError());
        return 3;
    }
    if (TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize Font System: %s", SDL_GetError());
        return 3;
    }
    
    RenderWindow window = RenderWindow(800,500,"Render Window");
    if (!window.IsInitialized()) return 3;


    Map* world[WorldWidth][WorldHeight];
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y] = new Map();
            world[x][y]->WallRectangle(MapWidth,MapHeight);
        }
    }
    int worldX = 1;
    int worldY = 1;
    int currentWorldX = worldX;
    int currentWorldY = worldY;


    // Generate World
    printf("Generating world...\n");
    //Map Gen -- Use png
    world[0][2]->FillRectangle(1,1,MapWidth,MapHeight,0);

    world[1][2]->FillRectangle(0,1,MapWidth,MapHeight,0);
    world[1][2]->FillRectangle(9, 5,10,11, 4);
    world[1][2]->FillRectangle(10,5,21,11, 2);
    world[1][2]->FillRectangle(21,5,22,11, 4);

    world[2][2]->FillRectangle(0,1,MapWidth-1,MapHeight,0);


    world[0][1]->FillRectangle(1,0,MapWidth,MapHeight,0);

    world[1][1]->FillRectangle(0,0,MapWidth,MapHeight,0);
    world[1][1]->FillRectangle(2,2,10,10, 2);   // Lake
    world[1][1]->FillRectangle(2,11,10,14,3);   // Lava
    world[1][1]->FillRectangle(32,5,35,11,4);   // Trees
    world[1][1]->FillRectangle(15,12,30,13,1);

    world[2][1]->FillRectangle(0,0,MapWidth-1,MapHeight,0);
    

    world[0][0]->FillRectangle(1,0,MapWidth,MapHeight-1,0);

    world[1][0]->FillRectangle(0,0,MapWidth,MapHeight-1,0);

    world[2][0]->FillRectangle(0,0,MapWidth-1,MapHeight-1,0);
    world[1][0]->FillRectangle(10,12,31,14,4);
    

    Shepherd* player = new Shepherd(20, 7);

    Entity* levelEntities[MaxEntities];
    for (int i = 0; i < MaxEntities; i++) {
        levelEntities[i] = nullptr;
    }

    printf("Loading level...\n");
    Map* currentLevel = LoadLevel(world, levelEntities, worldX, worldY, player->x, player->y);

    /*if (window.IsInitialized())
        window.SetDialogueText("Hit escape to exit program.");*/

    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;
    int ticks = 0;


    printf("Starting main loop...\n\n");

    SDL_Event event;
    while (true) {
        LastTick = CurrentTick;
        CurrentTick = SDL_GetPerformanceCounter();
        DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

        // Check Events
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;

        // Key Presses
        else if (event.type == SDL_KEYDOWN) {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_ESCAPE)
                break;
            else if (key == SDLK_F11)
                window.ToggleFullscreen();
            else if (key == SDLK_w || key == SDLK_UP) {
                Movement_Clear();
                MoveUp = true;
            } else if (key == SDLK_s || key == SDLK_DOWN) {
                Movement_Clear();
                MoveDown = true;
            } else if (key == SDLK_d || key == SDLK_RIGHT) {
                Movement_Clear();
                MoveRight = true;
            } else if (key == SDLK_a || key == SDLK_LEFT) {
                Movement_Clear();
                MoveLeft = true;
            } else if (key == SDLK_SPACE) {
                MoveFireballQueued = true;
            }
        } else if (event.type == SDL_KEYUP) {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_w || key == SDLK_UP || key == SDLK_s || key == SDLK_DOWN || key == SDLK_d || key == SDLK_RIGHT || key == SDLK_a || key == SDLK_LEFT)
                Move_QueueClear = true;
        }
        



        // Game Logic
        GameTick+=DeltaTime*TickRate;
        if (GameTick >= 1) {
            ticks++;
            window.LogTick();
            //SDL_Log("Game tick %i, DT %f", ticks, DeltaTime);

            player->animation = 0;

            if (MoveUp)
                Movement_ShiftPlayer(currentLevel, levelEntities, player, 0, 1, &worldX, &worldY);
            else if (MoveDown)
                Movement_ShiftPlayer(currentLevel, levelEntities, player, 0, -1, &worldX, &worldY);
            else if (MoveRight)
                Movement_ShiftPlayer(currentLevel, levelEntities, player, 1, 0, &worldX, &worldY);
            else if (MoveLeft)
                Movement_ShiftPlayer(currentLevel, levelEntities, player, -1, 0, &worldX, &worldY);

            if (Move_QueueClear)
                Movement_Clear();


            
            if (worldX >= WorldWidth)
                worldX = WorldWidth-1;
            else if (worldX < 0)
                worldX = 0;
            if (worldY >= WorldHeight)
                worldY = WorldHeight-1;
            else if (worldY < 0)
                worldY = 0;

            if (worldX != currentWorldX || worldY != currentWorldY) {
                currentWorldX = worldX;
                currentWorldY = worldY;
                currentLevel = LoadLevel(world, levelEntities, worldX, worldY, player->x, player->y);
            }


            // Check Entities for Fire
            if (currentLevel->tiles[player->x][player->y]->GetTileID() == 3)
                player->HasFire = true;


            // Toss Fireball
            if (MoveFireballQueued && player->HasFire) {
                MoveFireballQueued = false;
                player->HasFire = false;
                player->animation = 2;      //Fireball Toss animation
                
                //printf("Tossed fireball! Tick %i\n", ticks);
                AppendEntity(levelEntities, new Fireball(player->x, player->y, player->lastX, player->lastY, 0));
            } else
                MoveFireballQueued = false;



            // Tick Entities
            for (int i = 0; i < MaxEntities; i++) {
                if (!levelEntities[i]) continue;   //Skip checks if this is a nullpointer or uninitialized
                Entity* a = levelEntities[i];

                if (a->GetID() == 3) {      //Fireball
                    Fireball* fireball = dynamic_cast<Fireball*>(a);

                    if (fireball) {
                        int fX = a->x;
                        int fY = a->y;
                        Movement_ShiftEntity(currentLevel, levelEntities, a, fireball->speedX, fireball->speedY);

                        // Set things on fire

                        if (a->x == fX && a->y == fY) {     // Destroy fireball if did not move
                            RemoveEntity(levelEntities, fireball);
                            continue;
                        }
                    }
                } else if (a->GetID() == 2 && (ticks % 2) == 0) {   //Sheep AI
                    int dirToPlayerX = (player->x - a->x);
                    int dirToPlayerY = (player->y - a->y);
                    if (abs(dirToPlayerX) >= abs(dirToPlayerY))
                        dirToPlayerY = 0;
                    else
                        dirToPlayerX = 0;

                    // If there is one tile of space or less between sheep and player, don't move
                    if (!((abs(dirToPlayerX) <= 1 && dirToPlayerY == 0) || (dirToPlayerX == 0 && abs(dirToPlayerY) <= 1)))
                        Movement_ShiftEntity(currentLevel, levelEntities, a, sgn(dirToPlayerX), -sgn(dirToPlayerY));
                }
            }
            // Clean Entity List
            CleanEntities(levelEntities);



            GameTick = 0;
        }





        // Render
        SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
        SDL_RenderClear(window.canvas);
        window.UpdateSize();
        window.TickDeltaTime(DeltaTime);
        window.FillViewportBackground(10, 50, 10);
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                window.DrawTile(x,y,currentLevel->tiles[x][y]->GetTileID());
            }
        }
        for (Entity* obj : levelEntities) {
            if(obj)
                window.DrawEntity(obj->x, obj->y, obj->GetID(), obj->Flipped, obj->animation);
        }
        window.DrawEntity(player->x, player->y, player->GetID(), player->Flipped, player->animation);
        window.DrawDialogueBox();
        SDL_RenderPresent(window.canvas);
    }

    window.Close();
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y]->Free();
            delete world[x][y];
        }
    }
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i])
            delete levelEntities[i];
    }
    delete player;

    return 0;
}