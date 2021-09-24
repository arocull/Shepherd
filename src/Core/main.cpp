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

    SDL_Event* event = new SDL_Event();
    SDL_PollEvent(event);
    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;

    bool loadSave = false;
    if (SaveLoad::SaveGameValid()) { // If we have a save valid, load a save
        game->SaveMenuOpen();
        game->paused = true; // Pause game so we can interact with menus

        while (event->type != SDL_QUIT) { // Small loop for UI interaction
            LastTick = CurrentTick;
            CurrentTick = SDL_GetPerformanceCounter();
            DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

            // Check Events
            SDL_PollEvent(event);
            struct InputAction* input = game->ProcessInput(event);
            if (input->close || input->index == 1) { // Skip loading a save
                break;
            } else if (input->index == 0) { // Load a save if first option is selected
                loadSave = true;
                break;
            }
            delete input;

            game->DrawPauseMenu(DeltaTime);
        }
        window.LoadScreen(); // Draw load screen again as we do more work
        game->paused = false;
    }

    // Load save if it was declared to
    game->SaveMenuClose();
    if (loadSave) {
        loadSave = game->LoadGame(); // If the loaded save fails, default to a new game
    }
    if (!loadSave) {
        game->LoadGameDefaults();
    }

    // Main Game Loop -- Update tick to be latest frame
    CurrentTick = SDL_GetPerformanceCounter();
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
    }
    window.LoadScreen(); // Hide game as we clean up so the user know we're processing

    SaveLoad::Save(game->data); // Save game data
    delete game; // Free game and relevant game data
    delete event;
    window.Close(); // Close window and deallocate memory

    return 0;
}