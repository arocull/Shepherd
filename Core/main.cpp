#include <SDL2/SDL.h>
#include <cstdlib>

#include "Core/renderwindow.h"
#include "Core/map.h"

#include "Entities/entity.h"

#include "config.h"


bool MoveUp = false;
bool MoveDown = false;
bool MoveRight = false;
bool MoveLeft = false;
void Movement_Clear() {
    MoveUp = false;
    MoveDown = false;
    MoveRight = false;
    MoveLeft = false;
}

// Push entity from
/*void Movement_ShiftEntity(Map* world, Entity* obj, int dx, int dy) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y + dy;

}*/



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
    Map level = Map();
    level.WallRectangle(1,1);

    /*if (window.IsInitialized())
        window.SetDialogueText("Hit escape to exit program.");*/

    float LastTick = 0;
    float CurrentTick = SDL_GetPerformanceCounter();
    float DeltaTime = 0;
    float GameTick = 0;
    int ticks = 0;



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
            }
        } else if (event.type == SDL_KEYUP) {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_w || key == SDLK_UP || key == SDLK_s || key == SDLK_DOWN || key == SDLK_d || key == SDLK_RIGHT || key == SDLK_a || key == SDLK_LEFT)
                Movement_Clear();
        }



        // Game Logic
        GameTick+=DeltaTime*TickRate;
        if (GameTick >= 1) {
            ticks++;
            //SDL_Log("Game tick %i, DT %f", ticks, DeltaTime);
            GameTick = 0;
        }





        // Render
        SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
        SDL_RenderClear(window.canvas);
        window.UpdateSize();
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                window.DrawTile(x,y,level.tiles[x][y]->GetTileID());
            }
        }
        window.DrawDialogueBox();
        SDL_RenderPresent(window.canvas);
    }

    window.Close();
    level.Free();

    return 0;
}