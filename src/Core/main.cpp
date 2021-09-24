/* Shepherd - A small game by Alan O'Cull */

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <ctime>

#include "Core/config.h"
#include "Core/game.h"
#include "Core/Input/input_action.h"
#include "Core/Input/controller.h"
#include "Core/renderwindow.h"
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
    #ifndef DEBUG_MODE
    if (SaveLoad::SaveGameValid()) { // If we have a save valid, load a save
        game->SaveMenuOpen();

        while (event->type != SDL_QUIT) { // Small loop for UI interaction
            LastTick = CurrentTick;
            CurrentTick = SDL_GetPerformanceCounter();
            DeltaTime = (CurrentTick-LastTick) / SDL_GetPerformanceFrequency();

            game->paused = true; // Pause game so we can interact with menus

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

            game->DrawPauseMenu(DeltaTime); // Draw pause menu player interacts with
        }
        window.LoadScreen(); // Draw load screen again as we do more work
        game->paused = false;
    }
    #endif

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

    game->SaveGame(); // Save game data
    delete game; // Free game and relevant game data
    delete event;
    window.Close(); // Close window and deallocate memory

    return 0;
}