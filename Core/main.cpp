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
void Movement_ShiftEntity(Map* world, Entity* obj, int dx, int dy) {
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
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->tiles[obj->x][obj->y+yChange]->IsSolid())
                break;
            else
                obj->y+=yChange;
        }
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->tiles[obj->x+xChange][obj->y]->IsSolid())
                break;
            else
                obj->x+=xChange;
        }
    } else {
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->tiles[obj->x+xChange][obj->y]->IsSolid())
                break;
            else
                obj->x+=xChange;
        }
        for (int stepY = 0; stepY < distY; stepY++) {
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->tiles[obj->x][obj->y+yChange]->IsSolid())
                break;
            else
                obj->y+=yChange;
        }
    }

    //SDL_Log("Attempting step %i, %i with signs %i, %i\tNew Position: %i, %i", dx, dy, distX, distY, obj->x, obj->y);

    return;
}
void Movement_ShiftPlayer(Map* world, Shepherd* obj, int dx, int dy, int* worldX, int* worldY) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    obj->lastX = dx;
    obj->lastY = dy;

    if (desiredX >= MapWidth && *worldX+1 <= WorldWidth-1 && obj->HasAllSheep()) {
        *worldX = *worldX+1;
        obj->x = 0;
    } else if (desiredX < 0 && *worldX-1 >= 0 && obj->HasAllSheep()) {
        *worldX = *worldX-1;
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !world->tiles[desiredX][obj->y]->IsSolid())
        obj->x = desiredX;
    
    if (desiredY < 0 && *worldY+1 <= WorldHeight-1 && obj->HasAllSheep()) {
        *worldY = *worldY+1;
        obj->y = MapHeight-1;
    } else if (desiredY >= MapHeight && *worldY-1 >= 0 && obj->HasAllSheep()) {
        *worldY = *worldY-1;
        obj->y = 0;
    } else if (desiredY < MapHeight && desiredY >= 0 && !world->tiles[obj->x][desiredY]->IsSolid())
        obj->y = desiredY;
    
    return;
}



Map* LoadLevel(Map* world[WorldWidth][WorldHeight], Entity* levelEntities[MaxEntities], int worldX, int worldY) {
    //Dump entities in current level back into the map data (exclude sheep, fireballs)

    Map* newLevel = world[worldX][worldY];  //Point current level to the new map

    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i]) {
            delete levelEntities[i];
            levelEntities[i] = nullptr;
        }
    }

    //Load entities from map into the new level entities


    return newLevel;
}
void AppendEntity(Entity* entities[MaxEntities], Entity* newEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (!entities[i]) {
            entities[i] = newEntity;
            break;
        }
    }
}
void RemoveEntity(Entity* entities[MaxEntities], Entity* delEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && delEntity && entities[i] == delEntity) {
            delete entities[i];
            entities[i] = nullptr;
            break;
        }
    }
}
void CleanEntities(Entity* entities[MaxEntities]) {
    for (int i = MaxEntities-1; i > 0; i--) {
        if (!entities[i-1] && entities[i]) {
            entities[i-1] = entities[i];
            entities[i] = nullptr;
        }
    }
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
    Map* currentLevel = LoadLevel(world, levelEntities, worldX, worldY);

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

            if (MoveUp)
                Movement_ShiftPlayer(currentLevel, player, 0, 1, &worldX, &worldY);
            else if (MoveDown)
                Movement_ShiftPlayer(currentLevel, player, 0, -1, &worldX, &worldY);
            else if (MoveRight)
                Movement_ShiftPlayer(currentLevel, player, 1, 0, &worldX, &worldY);
            else if (MoveLeft)
                Movement_ShiftPlayer(currentLevel, player, -1, 0, &worldX, &worldY);

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
                currentLevel = LoadLevel(world, levelEntities, worldX, worldY);

                // Spawn Sheep
                AppendEntity(levelEntities, new Entity(player->x, player->y, 2));
            }


            // Check Entities for Fire
            if (currentLevel->tiles[player->x][player->y]->GetTileID() == 3)
                player->HasFire = true;


            // Toss Fireball
            if (MoveFireballQueued && player->HasFire) {
                MoveFireballQueued = false;
                player->HasFire = false;

                //Fireball fireball = Fireball(player->x, player->y, player->lastX, player->lastY, 0);
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
                        Movement_ShiftEntity(currentLevel, a, fireball->speedX, fireball->speedY);

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
                    Movement_ShiftEntity(currentLevel, a, sgn(dirToPlayerX), -sgn(dirToPlayerY));
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
                window.DrawEntity(obj->x, obj->y, obj->GetID(), false);
        }
        window.DrawEntity(player->x, player->y, player->GetID(), player->Flipped);
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