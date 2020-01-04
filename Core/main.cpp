/*
Written by Alan O'Cull
To compile:
$make clean
$make

To Debug:
$make run
$make debug

$gdb ./build/program
$> break FunctionNameToStopAt
$> run
*/


#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "Core/config.h"
#include "Core/mathutil.h"

#include "Core/renderwindow.h"

#include "Audio/sound_service.h"

#include "Map/tile.h"
#include "Map/map.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/shepherd.h"
#include "Entities/Subclasses/fireball.h"

#include "Entities/particle.h"

#include "Entities/entity_management.h"
#include "Entities/movement.h"
#include "Map/map_loading.h"

#include "Core/triggers.h"







int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize program: %s", SDL_GetError());
        return 3;
    }
    
    RenderWindow window = RenderWindow(800,500,"Shepherd");
    if (!window.IsInitialized()) return 3;

    SoundService soundService = SoundService();

    // Randomize seed based off of current time
    //srand(time(0));

    SDL_Event event;
    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;
    int ticks = 0;

    // Load world from files
    Map* world[WorldWidth][WorldHeight];
    world[0][2] = GenerateMapFromFile("Map/Maps/Desert/Desert1");
    world[0][1] = GenerateMapFromFile("Map/Maps/Desert/Desert2");
    world[0][0] = GenerateMapFromFile("Map/Maps/Desert/Desert3");

    world[1][2] = GenerateMapFromFile("Map/Maps/Desert/Desert4");
    world[1][1] = GenerateMapFromFile("Map/Maps/Desert/Desert5");
    world[1][0] = GenerateMapFromFile("Map/Maps/Desert/Desert6");

    world[2][2] = GenerateMapFromFile("Map/Maps/Desert/Desert7");
    world[2][1] = GenerateMapFromFile("Map/Maps/Desert/Desert8");
    world[2][0] = GenerateMapFromFile("Map/Maps/Desert/Desert9");

    world[3][2] = GenerateMapFromFile("Map/Maps/Desert/Desert10");
    world[3][1] = GenerateMapFromFile("Map/Maps/Desert/Desert11");
    world[3][0] = GenerateMapFromFile("Map/Maps/Desert/Desert10");

    int worldX = 1;
    int worldY = 1;
    int currentWorldX = worldX;
    int currentWorldY = worldY;
    
    Entity* levelEntities[MaxEntities];
    struct Particle* particles = (struct Particle*) calloc(MaxParticles, sizeof(Particle));

    Shepherd* player = new Shepherd(20, 8);
    levelEntities[0] = player;
    for (int i = 1; i < MaxEntities; i++) {
        levelEntities[i] = nullptr;
    }

    Map* currentLevel = LoadLevel(world, NULL, levelEntities, worldX, worldY, player->x, player->y);
    Trigger_GameStart(&window, &soundService, currentLevel, levelEntities);


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

            if (!player->Paused) {
                player->animation = 0;

                if (MoveUp)
                    Movement_ShiftPlayer(currentLevel, levelEntities, player, 0, 1, &worldX, &worldY);
                else if (MoveDown)
                    Movement_ShiftPlayer(currentLevel, levelEntities, player, 0, -1, &worldX, &worldY);
                else if (MoveRight)
                    Movement_ShiftPlayer(currentLevel, levelEntities, player, 1, 0, &worldX, &worldY);
                else if (MoveLeft)
                    Movement_ShiftPlayer(currentLevel, levelEntities, player, -1, 0, &worldX, &worldY);
            }

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
                StopParticles(particles);
                currentLevel = LoadLevel(world, currentLevel, levelEntities, worldX, worldY, player->x, player->y);
                Trigger_LevelLoaded(&window, &soundService, currentLevel, levelEntities);
            }


            // Check Entities for Fire
            int standingTile = currentLevel->GetTileID(player->x, player->y);
            if (standingTile == 3)
                player->HasFire = true;
            else if (standingTile < 0)
                Trigger_OnTile(&window, &soundService, currentLevel, levelEntities, abs(standingTile));


            // Toss Fireball
            if (MoveFireballQueued) {
                MoveFireballQueued = false;

                if (player->HasFire) {          // Sling Fireball
                    player->SlingFireball(levelEntities, particles);
                } else {        // Rally Sheep / Swing Attack
                    Trigger_StaffSwing(&window, &soundService, currentLevel, levelEntities);
                    player->SwingAttack(levelEntities, particles);
                }
            } else
                MoveFireballQueued = false;



            // Tick Entities and Tally Pressure Plates
            currentLevel->PressurePlatesPressed = 0;
            for (int i = 0; i < MaxEntities; i++) {
                if (!levelEntities[i] || levelEntities[i]->Paused) continue;   //Skip checks if this is a nullpointer or paused
                Entity* a = levelEntities[i];

                if (currentLevel->GetTileID(a->x, a->y) == 8)
                    currentLevel->PressurePlatesPressed++;

                if (a->GetID() == 3) {      //Fireball, move in a straight line
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


                } else if (a->GetID() == 2 && (ticks % 2) == 0) {   //Sheep AI; follow player every other tick
                    a->animation = 0;

                    int dirToPlayerX = (player->x - a->x);
                    int dirToPlayerY = (player->y - a->y);
                    if (abs(dirToPlayerX) >= abs(dirToPlayerY))
                        dirToPlayerY = 0;
                    else
                        dirToPlayerX = 0;

                    Movement_ShiftEntity(currentLevel, levelEntities, a, sgn(dirToPlayerX), -sgn(dirToPlayerY));


                } else if (a->GetID() == 4) {   //Wolf AI, hunt down closest sheep--if none left, attack player
                    Wolf* wolf = dynamic_cast<Wolf*>(a);
                    if (wolf) {
                        if (wolf->InStun()) {
                            wolf->TickStun();
                        } else if (!wolf->IsHunting()) {
                            Entity* target = GetNearestEntity(levelEntities, a->x, a->y, MaxEntities, 2);
                            if (!target)
                                target = player;
                            
                            if (target)
                                wolf->InitiateHunt(target);
                            else
                                a->animation = 0;
                        } else {
                            Entity* target = wolf->GetTarget();
                            int dirToTargetX = (target->x - a->x);
                            int dirToTargetY = (target->y - a->y);
                            if (abs(dirToTargetX) >= abs(dirToTargetY))
                                dirToTargetY = 0;
                            else
                                dirToTargetX = 0;

                            if (distF((float) wolf->x, (float) wolf->y, (float) target->x, (float) target->y) <= 1.1f) {
                                target->TakeDamage(1, wolf);
                                wolf->EndHunt();
                            }

                            Movement_ShiftEntity(currentLevel, levelEntities, a, sgn(dirToTargetX), -sgn(dirToTargetY));
                            a->animation = 1;
                        }       
                    }
                }
            }
            // Clean Entity List
            CleanEntities(levelEntities);



            GameTick = 0;
        }


        // Particles
        TickParticles(particles, DeltaTime);


        // Render
        SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
        SDL_RenderClear(window.canvas);
        window.UpdateSize();
        window.TickDeltaTime(DeltaTime);

        // Fill background based off of biome
        switch (currentLevel->GetMapBiome()) {
            case 'D':   // Desert
                window.FillViewportBackground(220, 220, 100);
                break;
            default:    // Default / Forest
                window.FillViewportBackground(10, 60, 20);
        }

        // Draw tiles first
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                window.DrawTile(x,y,currentLevel->GetTileID(x,y));
            }
        }

        // Draw all entities aside from Shepherd
        for (Entity* obj : levelEntities) {
            if (obj && obj->GetID() != 1)
                window.DrawEntity(obj->x, obj->y, obj->GetID(), obj->Flipped, obj->animation, obj->animationMetadata);
        }


        // Draw active particles - enable blending for transparency
        SDL_BlendMode blend;
        SDL_GetRenderDrawBlendMode(window.canvas, &blend);
        SDL_SetRenderDrawBlendMode(window.canvas, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < MaxParticles; i++) {
            if (particles[i].active)
                window.DrawParticle(particles[i].x, particles[i].y, particles[i].id, particles[i].lifetime/particles[i].maxLifetime);
        }
        SDL_SetRenderDrawBlendMode(window.canvas, blend);

        // Draw shepherd last
        window.DrawEntity(player->x, player->y, player->GetID(), player->Flipped, player->animation, player->animationMetadata);

        // Draw GUI
        window.DrawDialogueBox();
        
        SDL_RenderPresent(window.canvas);
        soundService.Tick(DeltaTime);
    }


    // Close window and deallocate memory
    window.Close();
    soundService.CloseSoundService();

    StopParticles(particles);
    free(particles);

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