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
#include <SDL2/SDL_mixer.h>
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
#include "Entities/Subclasses/torch.h"

#include "Entities/particle.h"

#include "Entities/entity_management.h"
#include "Entities/movement.h"
#include "Entities/AI/path_manager.h"
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
    #ifdef DEBUG_MODE
        bool accelerate = false;
    #endif
    bool GamePaused = false;

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
    world[3][0] = GenerateMapFromFile("Map/Maps/Desert/Desert12");

    world[4][2] = GenerateMapFromFile("Map/Maps/Desert/Pyramid13");
    world[4][1] = GenerateMapFromFile("Map/Maps/Desert/Pyramid14");
    world[4][0] = GenerateMapFromFile("Map/Maps/Desert/Pyramid15");

    world[5][2] = GenerateMapFromFile("Map/Maps/Desert/Pyramid16");
    world[5][1] = GenerateMapFromFile("Map/Maps/Desert/Pyramid17");
    world[5][0] = GenerateMapFromFile("Map/Maps/Desert/Pyramid18");

    world[6][2] = GenerateMapFromFile("Map/Maps/Desert/Pyramid19");
    world[6][1] = GenerateMapFromFile("Map/Maps/Desert/Pyramid20");
    world[6][0] = GenerateMapFromFile("Map/Maps/Desert/Pyramid21");


    // Perform first-time setup for levels that need it (set up puzzles, update entity data)
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            Trigger_SetupPuzzles(world[x][y]);
        }
    }

    // World indexing, current X and Y is the currently loaded level, world X and Y is changed based upon movement and says what level to load
    int worldX = 1;
    int worldY = 1;
    int currentWorldX = worldX;
    int currentWorldY = worldY;
    
    // Allocate memory for runtime entities
    Entity* levelEntities[MaxEntities];
    for (int i = 1; i < MaxEntities; i++) {
        levelEntities[i] = nullptr;
    }
    
    struct Particle* particles = (struct Particle*) calloc(MaxParticles, sizeof(Particle));

    // Player setup
    Shepherd* player = new Shepherd(20, 8);
    levelEntities[0] = player;

    // Load in current level and (finally) start the game
    Map* currentLevel = LoadLevel(world, NULL, levelEntities, worldX, worldY, player->x, player->y);
    Trigger_GameStart(&window, &soundService, currentLevel, levelEntities);


    while (event.type != SDL_QUIT) {
        LastTick = CurrentTick;
        CurrentTick = SDL_GetPerformanceCounter();
        DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

        // Check Events
        SDL_PollEvent(&event);

        // Key Presses
        if (event.type == SDL_KEYDOWN) {
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
            #ifdef DEBUG_MODE   // In Debug Mode, we can hold Left Shift to accelerate the game
                if (key == SDLK_LSHIFT)
                    accelerate = true;
            #endif
        } else if (event.type == SDL_KEYUP) {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_w || key == SDLK_UP || key == SDLK_s || key == SDLK_DOWN || key == SDLK_d || key == SDLK_RIGHT || key == SDLK_a || key == SDLK_LEFT)
                Move_QueueClear = true;
            #ifdef DEBUG_MODE
                if (key == SDLK_LSHIFT)
                    accelerate = false;
            #endif
        }
        



        // Game Logic
        if (!GamePaused) {
            #ifdef DEBUG_MODE
                if (accelerate)
                    GameTick+=DeltaTime*TickRate*TickAcceleration;
                else
                    GameTick+=DeltaTime*TickRate;
            #else
                GameTick+=DeltaTime*TickRate;
            #endif
        }
        if (GameTick <= 0.5f && Move_QueueClear && !GamePaused)    // Prevent any 'sliding'
            Movement_Clear();
        else if (GameTick >= 1) {
            ticks++;
            window.LogTick();

            player->ticksIdled++;
            if (player->ticksIdled == TicksUntilIdle)   //If the player sits still, feed them hints or story
                Trigger_Idled(&window, &soundService, currentLevel, levelEntities);

            if (!player->Paused) {      //If they player is not paused, let them move if input is given
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
                Trigger_LevelLoaded(&window, &soundService, world, currentLevel, levelEntities);
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
                    soundService.PlaySound("Audio/Resources/FireballSling.wav");
                } else {        // Rally Sheep / Swing Attack
                    player->SwingAttack(levelEntities, particles);
                    Trigger_StaffSwing(&window, &soundService, currentLevel, levelEntities);
                }
                player->ticksIdled = 0;
            } else
                MoveFireballQueued = false;



            // Tick Entities and Tally Pressure Plates
            bool currentPuzzleStatus = currentLevel->PuzzleStatus;
            bool PressurePlatesChanged = false;
            currentLevel->PressurePlatesPressed = 0;
            for (int i = 0; i < MaxEntities; i++) {
                if (!levelEntities[i]) continue;   //Skip checks if this is a nullpointer or paused
                Entity* a = levelEntities[i];

                a->Tick();
                if (a->HasFire && a->HasFrost)  // Fire overrides frost
                    a->HasFrost = false;
                
                if (a->HasFire || a->HasFrost)  // Freeze or thaw nearby water
                    currentLevel->FreezeArea(a->x, a->y, 1, a->HasFire);

                if (currentLevel->GetTileID(a->x, a->y) == 8) {
                    if (!a->OnPressurePlate) {
                        a->OnPressurePlate = true;
                        Particle* clickEffect = ActivateParticle(particles, 3, a->x, a->y);

                        PressurePlatesChanged = true;
                    }
                    currentLevel->PressurePlatesPressed++;
                } else if (a->OnPressurePlate) {
                    a->OnPressurePlate = false;
                    PressurePlatesChanged = true;
                }

                if (a->Paused) continue;

                

                if (a->GetID() == 3) {      //Fireball, move in a straight line
                    Fireball* fireball = dynamic_cast<Fireball*>(a);

                    if (fireball) {
                        int fX = a->x;
                        int fY = a->y;
                        Movement_ShiftEntity(currentLevel, levelEntities, a, fireball->speedX, fireball->speedY);

                        // Set things on fire that the fireball is ontop of

                        if (a->x == fX && a->y == fY) {     // Destroy fireball if did not move
                            fireball->Burst(levelEntities, particles);
                            continue;
                        }
                    }


                } else if (a->GetID() == 2 && ((ticks % 2) == 0 || distGrid(a->x, a->y, player->x, player->y) > 4)) {   //Sheep AI; follow player every other tick
                    a->animation = 0;

                    Sheep* sheep = dynamic_cast<Sheep*>(a);

                    int dirX = 0;
                    int dirY = 0;

                    CheckPathObscurity(sheep->currentPath, currentLevel, levelEntities, true);
                    if (!sheep->currentPath || !sheep->currentPath->complete || !(sheep->currentPath->goalX == player->x && sheep->currentPath->goalY == player->y)) {
                        Path_FreePath(sheep->currentPath);
                        sheep->currentPath = GetPath(a->x, a->y, player->x, player->y, currentLevel, levelEntities, true);
                    }
                    GetNextMovement(a->x, a->y, sheep->currentPath, &dirX, &dirY);

                    Movement_ShiftEntity(currentLevel, levelEntities, a, dirX, -dirY);

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

                            if (distGrid(wolf->x, wolf->y, target->x, target->y) <= 1) {
                                target->TakeDamage(1, wolf);
                                wolf->EndHunt();
                            }

                            int dirX = 0;
                            int dirY = 0;

                            CheckPathObscurity(wolf->currentPath, currentLevel, levelEntities, false);
                            if (!wolf->currentPath || !wolf->currentPath->complete || !(wolf->currentPath->goalX == target->x && wolf->currentPath->goalY == target->y)) {
                                Path_FreePath(wolf->currentPath);
                                wolf->currentPath = GetPath(a->x, a->y, player->x, player->y, currentLevel, levelEntities, false);
                            }
                            GetNextMovement(a->x, a->y, wolf->currentPath, &dirX, &dirY);

                            Movement_ShiftEntity(currentLevel, levelEntities, a, dirX, -dirY);
                            a->animation = 1;
                        }       
                    }
                }
            }
            // Clean Entity List
            CleanEntities(levelEntities);

            if (PressurePlatesChanged)
                Trigger_PuzzleInput(&window, &soundService, particles, currentLevel, levelEntities);

            int puzzlesEnabled = 0;
            int puzzlesCompleted = 0;
            for (int i = 0; i < MaxPuzzles; i++) {
                if (currentLevel->Puzzles[i].Enabled) {
                    puzzlesEnabled++;

                    Puzzle_CheckSolution(&currentLevel->Puzzles[i]);
                    if (currentLevel->Puzzles[i].Solved)
                        puzzlesCompleted++;
                }
            }
            if (puzzlesEnabled > 0)
                currentLevel->PuzzleStatus = (puzzlesEnabled == puzzlesCompleted);

            // If the player completed a puzzle this tick, show a particle
            if (!currentPuzzleStatus && currentLevel->PuzzleStatus) {
                Particle* a = ActivateParticle(particles, 2, player->x, player->y);
                a->maxLifetime = 0.75f;
            }

            GameTick--;
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
                window.FillViewportBackground(210, 200, 80);
                break;
            case 'P':   // Pyramid
                window.FillViewportBackground(100, 80, 65);
                break;
            case 'M':   // Mountain
                window.FillViewportBackground(20, 20, 20);
                break;
            case 'S':   // Snowy
                window.FillViewportBackground(200, 210, 220);
                break;
            default:    // Default / Forest
                window.FillViewportBackground(10, 60, 20);
        }

        // Draw tiles first
        int currentTileID = 0;
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                currentTileID = currentLevel->GetTileID(x,y);
                window.DrawTile(
                    x,
                    y,
                    currentTileID,
                    IsTileable(currentTileID) ? GetTilingIndex(
                        currentLevel->GetTileIDConstrained(x,y-1) == currentTileID,
                        currentLevel->GetTileIDConstrained(x,y+1) == currentTileID,
                        currentLevel->GetTileIDConstrained(x+1,y) == currentTileID,
                        currentLevel->GetTileIDConstrained(x-1,y) == currentTileID
                    ) : 0);
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
        window.DrawStatusBar(player->GetHealth(), currentLevel->PuzzleStatus);
        window.DrawDialogueBox();
        if (GamePaused)
            window.DrawPauseMenu(0);
        
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