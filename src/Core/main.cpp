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

#include "Core/game.h"

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
#include "Entities/Subclasses/lever.h"
#include "Entities/Subclasses/spirit.h"

#include "Entities/particle.h"

#include "Entities/entity_management.h"
#include "Entities/movement.h"
#include "Entities/AI/path_manager.h"
#include "Entities/AI/ai_manager.h"

#include "Triggers/triggers.h"

#include "Map/map_loading.h"
#include "Map/save.h"


int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize program: %s", SDL_GetError());
        return 3;
    }
    
    RenderWindow window = RenderWindow(800,500,"Shepherd");
    if (!window.IsInitialized()) return 3;

    Game* game = new Game(&window);

    SDL_Event* event;
    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;

    SDL_PollEvent(event);
    while (event->type != SDL_QUIT) {
        LastTick = CurrentTick;
        CurrentTick = SDL_GetPerformanceCounter();
        DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

        // Check Events
        SDL_PollEvent(event);
        struct InputAction* input = game->ProcessInput(event);
        if (input->close) { // Close program if should-close
            break;
        }
        delete input;

        if (game->paused) { // If the game is paused, just render GUI and skip ahead
            game->DrawPauseMenu(DeltaTime);
            continue;
        }

        bool doTick = game->Step(DeltaTime);
        if (doTick) {
            game->Tick();
        }
        game->Draw(DeltaTime);


        // OLD CODE HERE
        // Game Logic
            // Check Entities for Fire
            int standingTile = currentLevel->GetTileID(player->x, player->y);
            if (standingTile >= TileID::ET_Trigger1 && standingTile <= TileID::ET_Trigger4) {
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

                if (a->Paused) {
                    a->lastX = a->x;
                    a->lastY = a->y;
                    continue;
                }
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
                } else if (a->GetID() == EntityID::EE_PyramidGolem) {
                    PyramidGolem* boss = dynamic_cast<PyramidGolem*>(a);
                    Movement_ShiftEntity(currentLevel, levelEntities, boss, boss->GetGoalX(), boss->GetGoalY(), true);
                    boss->Flipped = false;

                    if (boss->DoFireballToss()) {
                        Fireball* fireball = new Fireball(boss->x, boss->y+1, 0, -1, 0);
                        fireball->enemy = true;
                        AppendEntity(levelEntities, fireball);
                        window.AddScreenShake(0.4f, 0);
                        soundService.PlaySound("Assets/Audio/FireballSling.wav");
                    }

                    if (boss->ShouldBridge()) {
                        boss->BuildBridge(currentLevel);
                        window.AddScreenShake(0, 0.7f); // Boss was hit, give satisfaction
                    }

                    if (boss->ShouldSmash() || boss->GetHealth() <= 0) {
                        boss->Smash(currentLevel, levelEntities);
                        window.AddScreenShake(0.9f, 0); // Boss slammed ground
                        Particle* smash = ActivateParticle(particles, ParticleID::EP_GolemSmash, boss->x, boss->y);
                        smash->maxLifetime = 0.7f;
                    }

                    // Open doors
                    if (boss->GetHealth() <= 0) {
                        window.SetDialogueText("A sigh of relief...", 0);
                        currentLevel->FillRectangle(0, 11, 1, 14, TileID::ET_Fizzler);
                        currentLevel->FillRectangle(39, 11, 40, 14, TileID::ET_Fizzler);
                    }
                } else if (a->GetID() == EntityID::EE_Spirit) {
                    Spirit* spirit = dynamic_cast<Spirit*>(a);
                    if (spirit) spirit->Emit(particles);
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
                if (a) a->maxLifetime = 0.75f;
            }

            GameTick--;
        }
    }


    // Free world, entities, and finally the player
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            SaveLoad::SaveMap(world[x][y], x, y);
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

    delete game;

    // Close window and deallocate memory
    window.Close();

    return 0;
}