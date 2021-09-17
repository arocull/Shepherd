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
    }

    SaveLoad::Save(game->data); // Save game data
    delete game; // Free game and relevant game data
    window.Close(); // Close window and deallocate memory

    return 0;
}