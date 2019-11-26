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

#include "config.h"
#include "mathutil.h"

#include "Core/renderwindow.h"

#include "Map/tile.h"
#include "Map/map.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/shepherd.h"
#include "Entities/Subclasses/fireball.h"

#include "Entities/entity_management.h"
#include "Entities/movement.h"
#include "Map/map_loading.h"







int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize program: %s", SDL_GetError());
        return 3;
    }
    /*if (TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize Font System: %s", SDL_GetError());
        return 3;
    }*/
    
    RenderWindow window = RenderWindow(800,500,"Shepherd");
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
    //Map Gen -- Use png in future for maps
    world[0][2]->FillRectangle(1,1,MapWidth,MapHeight,0);

    world[1][2]->FillRectangle(0,1,MapWidth,MapHeight,0);
    world[1][2]->FillRectangle(9, 5,10,11, 4);  // Trees
    world[1][2]->FillRectangle(10,5,21,11, 2);  // Lake
    world[1][2]->FillRectangle(21,5,22,11, 4);  // Trees

    world[2][2]->FillRectangle(0,1,MapWidth-1,MapHeight,0);


    world[0][1]->FillRectangle(1,0,MapWidth,MapHeight,0);

    world[1][1]->FillRectangle(0,0,MapWidth,MapHeight,0);
    world[1][1]->FillRectangle(2,2,10,10, 2);   // Lake
    world[1][1]->FillRectangle(2,11,10,14,3);   // Lava
    world[1][1]->FillRectangle(32,5,35,11,4);   // Trees
    world[1][1]->FillRectangle(15,12,30,13,1);

    world[2][1]->FillRectangle(0,0,MapWidth-1,MapHeight,0);
    

    //world[0][0]->FillRectangle(1,0,MapWidth,MapHeight-1,0);
    delete world[0][0];
    world[0][0] = GenerateMapFromFile("Map/Maps/ZeroZero");

    world[1][0]->FillRectangle(0,0,MapWidth,MapHeight-1,0);

    world[2][0]->FillRectangle(0,0,MapWidth-1,MapHeight-1,0);
    world[1][0]->FillRectangle(10,12,31,14,4);  // Trees
    
    // Summon player
    Shepherd* player = new Shepherd(20, 7);

    Entity* levelEntities[MaxEntities];
    levelEntities[0] = player;
    for (int i = 1; i < MaxEntities; i++) {
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
    //window.SetDialogueText("ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890 .,!?()-+*/=_");
    window.SetDialogueText("Basic dialogue text.\nThis feature is currently being tested.");

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


            // Limit world coordinates to actual world
            if (worldX >= WorldWidth)
                worldX = WorldWidth-1;
            else if (worldX < 0)
                worldX = 0;
            if (worldY >= WorldHeight)
                worldY = WorldHeight-1;
            else if (worldY < 0)
                worldY = 0;

            // If world coordinate has changed, load a new level
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
                    //if (!((abs(dirToPlayerX) == 1 && dirToPlayerY == 0) || (dirToPlayerX == 0 && abs(dirToPlayerY) == 1)))
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

        //window.WriteText(0, 0, 1680, 20, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890 .,!?()-+*/=_");
        //window.WriteText(0, 20, 1680, 40, "The quick brown fox jumped over the lazy dog.");

        window.DrawDialogueBox();
        
        SDL_RenderPresent(window.canvas);
    }


    // Close window and deallocate memory
    window.Close();
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y]->Free();
            delete world[x][y];
        }
    }
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1)
            delete levelEntities[i];
    }
    delete player;

    return 0;
}