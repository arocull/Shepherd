/*
Written by Alan O'Cull
To compile:
$ make clean
$ make

To Run: $ make run

$ make debug
$ > break FunctionNameToStopAt
$ > run

$ make debug-mem // Basic memory tracking
$ make debug-mem-heavy // Attempts to locate all memory leaks
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "Core/config.h"
#include "Core/mathutil.h"

#include "Core/Input/input_action.h"
#include "Core/Input/controller.h"
#include "Core/UI/menu.h"
#include "Core/UI/menu_manager.h"
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
#include "Entities/AI/ai_manager.h"
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

    MenuManager* menus = new MenuManager(); // Should this be a pointer, or created as a normal base-object like SoundService and Window?
    Controller* controller = new Controller(menus, &soundService); // Player controller input (done as pointer in-case multiple are used in future)

    AIManager* ai = new AIManager();

    SDL_Event event;
    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;
    int ticks = 0;
    bool GamePaused = false;

    // Load world from files
    Map* world[WorldWidth][WorldHeight];
    world[0][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Start1");
    world[1][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Start2");
    world[2][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Start3");
    world[3][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Start4");

    world[0][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert1");
    world[1][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert2");
    world[2][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert3");
    world[3][1] = GenerateMapFromFile("Map/Maps/DesertTrek/GateArea");

    world[0][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert4");
    world[1][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert5");
    world[2][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert6");
    world[3][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Desert7");

    world[4][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Empty");
    world[4][1] = GenerateMapFromFile("Map/Maps/DesertTrek/PyramidHall");
    world[4][0] = GenerateMapFromFile("Map/Maps/DesertTrek/DesertScroll");

    world[5][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid1");
    world[6][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid2");
    world[7][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid3");

    world[5][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid4");
    world[6][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid5");
    world[7][1] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid6");

    world[5][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid7");
    world[6][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid8");
    world[7][0] = GenerateMapFromFile("Map/Maps/DesertTrek/Pyramid9");

    world[8][2] = GenerateMapFromFile("Map/Maps/DesertTrek/Empty");
    world[8][1] = GenerateMapFromFile("Map/Maps/DesertTrek/PyramidFireballCatching");
    world[8][0] = GenerateMapFromFile("Map/Maps/DesertTrek/PyramidScroll");

    // Perform first-time setup for levels that need it (set up puzzles, update entity data, get scrolls)
    {
        int numScrolls = 0;
        for (int x = 0; x < WorldWidth; x++) {
            for (int y = 0; y < WorldHeight; y++) {
                Trigger_SetupPuzzles(world[x][y]);
                if (world[x][y]->HasScroll()) numScrolls++;
            }
        }

        menus->InitScrolls(numScrolls);
        int currentScroll = 0;

        // TODO: Obscure undiscovered scrolls until found
        for (int x = 0; x < WorldWidth && currentScroll < numScrolls; x++) {
            for (int y = 0; y < WorldHeight && currentScroll < numScrolls; y++) {
                if (world[x][y]->HasScroll()) {
                    world[x][y]->SetScrollIndex(currentScroll);
                    currentScroll++;
                }
            }
        }
    }

    // World indexing, current X and Y is the currently loaded level, world X and Y is changed based upon movement and says what level to load
    int worldX = 0;
    int worldY = 2;
    #ifdef DEBUG_MODE
        if (DEBUG_SkipGates == 2) {
            worldX = 4;
            worldY = 1;
        } else if (DEBUG_SkipGates >= 3) {
            worldX = 7;
            worldY = 1;
        }
    #endif
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


    SDL_PollEvent(&event);
    while (event.type != SDL_QUIT) {
        LastTick = CurrentTick;
        CurrentTick = SDL_GetPerformanceCounter();
        DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

        // Check Events
        SDL_PollEvent(&event);

        // Process Input (movement applied automatically using globals, window-specific functions happen here)
        struct InputAction inputAction;
        controller->ProcessInput(&event, &inputAction, GamePaused, window.InFullscreen());
        if (inputAction.close) break; // Close program
        GamePaused = inputAction.pause; // Pause / Unpause game
        if (inputAction.fullscreen != window.InFullscreen()) { // Toggle fullscreen
            window.ToggleFullscreen(inputAction.fullscreen);
            menus->ToggleFullscreen(inputAction.fullscreen);
        }



        if (GamePaused) { // If the game is paused, just render GUI and skip ahead
            SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
            SDL_RenderClear(window.canvas);
            window.UpdateSize();

            // TODO: Currently this is a lot of pointers to pointers to pointers, maybe clean up a bit?
            window.DrawStatusBar(player->GetHealth(), currentLevel->PuzzleStatus);
            window.DrawDialogueBox(menus->activeMenu->optionDesc[menus->activeMenu->optionIndex]);
            window.DrawMenuBackground();
            window.DrawMenu(menus->pauseMenu->getNumOptions(), menus->pauseMenu->optionNames, menus->pauseMenu->optionIndex, menus->inSubmenu());

            if (menus->inSubmenu()) { // Draw submenu as well if it is active
                window.DrawMenu(menus->activeMenu->getNumOptions(), menus->activeMenu->optionNames, menus->activeMenu->optionIndex, false);
            }

            soundService.Tick(DeltaTime);
            SDL_RenderPresent(window.canvas);
            continue;
        }

        // Game Logic
        #ifdef DEBUG_MODE
            if (controller->accelerate)
                GameTick+=DeltaTime*TickRate*TickAcceleration;
            else
                GameTick+=DeltaTime*TickRate;
        #else
            GameTick+=DeltaTime*TickRate;
        #endif
        if (GameTick <= 0.5f && Move_QueueClear)    // Prevent any 'sliding'
            Movement_Clear();
        else if (GameTick >= 1) {
            ticks++;
            window.LogTick();

            // Tally sheep
            int sheepLeft = 0;
            for (int i = 0; i < MaxEntities && sheepLeft < MaxSheep; i++) {
                if (levelEntities[i] && levelEntities[i]->GetID() == EntityID::EE_Sheep) sheepLeft++;
            }
            if (sheepLeft < MaxSheep) {
                Trigger_GameOver(&window, &soundService, currentLevel, levelEntities);
                player->Paused = true;
            }

            player->ticksIdled++;
            if (player->ticksIdled == TicksUntilIdle && sheepLeft >= MaxSheep) {   //If the player sits still, feed them hints or story
                Trigger_Idled(&window, &soundService, currentLevel, levelEntities);
            }

            if (!player->Paused) {      //If they player is not paused, let them move if input is given
                player->animation = 0;

                player->lastX = player->x;
                player->lastY = player->y;

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
                player->lastX = player->x; // Don't draw player teleporting
                player->lastY = player->y;
            }


            // Check Entities for Fire
            int standingTile = currentLevel->GetTileID(player->x, player->y);
            if (standingTile < 0) {
                Trigger_OnTile(&window, &soundService, currentLevel, levelEntities, abs(standingTile));
            } else if (standingTile == TileID::ET_Fizzler) {
                Trigger_OnFizzler(&window, &soundService, currentLevel, player);
            } else if (standingTile == TileID::ET_Scroll && currentLevel->HasScroll()) {
                Trigger_OnScroll(&window, &soundService, currentLevel, levelEntities); // Do scroll trigger
                menus->UnlockScroll(currentLevel->GetScrollIndex(), currentLevel->GetScrollName(), currentLevel->GetScroll()); // Unlock scroll in index
            }

            // Toss Fireball
            if (MoveFireballQueued && sheepLeft >= MaxSheep) {
                MoveFireballQueued = false;
                player->SwingAttack(levelEntities, particles, &soundService);
                Trigger_StaffSwing(&window, &soundService, currentLevel, levelEntities);
                player->ticksIdled = 0;
            } else MoveFireballQueued = false;

            ai->SetContext(player, currentLevel, levelEntities, ticks);

            // Tick Entities and Tally Pressure Plates
            bool currentPuzzleStatus = currentLevel->PuzzleStatus;
            bool PressurePlatesChanged = false;
            bool LeversChanged = false;
            currentLevel->PressurePlatesPressed = 0;
            for (int i = 0; i < MaxEntities; i++) {
                if (!levelEntities[i]) continue;   //Skip checks if this is a nullpointer or paused
                Entity* a = levelEntities[i];

                if (a->GetID() != EntityID::EE_Shepherd) { // Update last positions
                    if (!a->shovedX) a->lastX = a->x;
                    else a->shovedX = false;
                    if (!a->shovedY) a->lastY = a->y;
                    else a->shovedY = false;
                }

                a->Tick();
                if (a->HasFire && a->HasFrost)  // Fire overrides frost
                    a->HasFrost = false;
                
                if (a->HasFire || a->HasFrost)  // Freeze or thaw nearby water
                    currentLevel->FreezeArea(a->x, a->y, 1, a->HasFire);

                int entityTile = currentLevel->GetTileID(a->x, a->y);
                if (entityTile == TileID::ET_Pressure_Plate) {
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

                // If standing on lava, take burn damage, gain fire, lose frost
                if (entityTile == TileID::ET_Magma && a->GetID() != EntityID::EE_Fireball) {
                    a->TakeDamage(1, nullptr);
                    a->HasFire = true;
                    a->HasFrost = false;
                }

                if (a->Paused) continue;
                ai->TickAI(a);
                

                if (a->GetID() == EntityID::EE_Fireball && a->GetHealth() > 0) { //Fireball, move in a straight line
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
                } else if (a->GetID() == EntityID::EE_Lever) {
                    Lever* lever = dynamic_cast<Lever*>(a);
                    if (lever->stateChanged) LeversChanged = true;
                }
            }
            // Clean Entity List
            ai->ClearContext();
            CleanEntities(levelEntities);

            bool doTriggerPuzzleInput = false;

            // Run checks on puzzles
            int puzzlesEnabled = 0;
            int puzzlesCompleted = 0;
            for (int i = 0; i < MaxPuzzles; i++) {
                if (currentLevel->Puzzles[i].Enabled) {
                    puzzlesEnabled++;

                    bool wasSolved = currentLevel->Puzzles[i].Solved;

                    Puzzle_CheckSolution(&currentLevel->Puzzles[i]);
                    if (currentLevel->Puzzles[i].Solved) {
                        puzzlesCompleted++;
                        if (wasSolved != currentLevel->Puzzles[i].Solved) {
                            doTriggerPuzzleInput = true;
                            #ifdef DEBUG_MODE
                            printf("Puzzle solved!\n");
                            #endif
                        }
                    }
                }
            }
            if (puzzlesEnabled > 0)
                currentLevel->PuzzleStatus = (puzzlesEnabled == puzzlesCompleted);

            // Run triggers
            if (doTriggerPuzzleInput || PressurePlatesChanged || LeversChanged) Trigger_PuzzleInput(&window, &soundService, particles, currentLevel, levelEntities);

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
                    IsTileable(currentTileID) ? GetTilingIndex( // If the tile is tileable, get the tiling index
                        currentLevel->GetTileIDConstrained(x,y-1) == currentTileID,
                        currentLevel->GetTileIDConstrained(x,y+1) == currentTileID,
                        currentLevel->GetTileIDConstrained(x+1,y) == currentTileID,
                        currentLevel->GetTileIDConstrained(x-1,y) == currentTileID
                    ) : 0); // Otherwise, simply return zero
            }
        }

        // Draw all entities aside from Shepherd
        for (Entity* obj : levelEntities) {
            if (obj && obj->GetID() != EntityID::EE_Shepherd)
                window.DrawEntity(obj->x, obj->y, obj->lastX, obj->lastY, obj->GetID(), obj->Flipped, obj->animation, obj->animationMetadata);
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
        window.DrawEntity(player->x, player->y, player->lastX, player->lastY, player->GetID(), player->Flipped, player->animation, player->animationMetadata);

        // Draw GUI
        window.DrawStatusBar(player->GetHealth(), currentLevel->PuzzleStatus);
        window.DrawDialogueBox();
        
        soundService.Tick(DeltaTime);
        SDL_RenderPresent(window.canvas);
    }


    // Close window and deallocate memory
    window.Close();
    soundService.CloseSoundService();

    // Free menus
    menus->Free();
    delete menus;
    delete controller;

    // Free in-game effects
    StopParticles(particles);
    free(particles);

    // Free world, entities, and finally the player
    ai->ClearContext();
    delete ai;

    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y]->Free();
            delete world[x][y];
        }
    }
    for (int i = 0; i < MaxEntities; i++) {
        if (levelEntities[i] && levelEntities[i]->GetID() != 1) {
            levelEntities[i]->Unload();
            delete levelEntities[i];
        }
    }
    delete player;

    return 0;
}