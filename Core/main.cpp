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

#include "config.h"


bool MoveUp = false;
bool MoveDown = false;
bool MoveRight = false;
bool MoveLeft = false;
bool Move_QueueClear = false;
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

    int distX = abs(obj->x - (obj->x + dx));
    int distY = abs(obj->y - (obj->y - dy));

    int xChange = significand(dx);
    int yChange = -significand(dy);

    if (distY > distX) {
        for (int stepY = 0; stepY < distY; stepY++) {
            if (world->tiles[obj->x][obj->y+yChange]->IsSolid())
                break;
            else
                obj->y+=yChange;
        }
        for (int stepX = 0; stepX < distX; stepX++) {
            if (world->tiles[obj->x+xChange][obj->y]->IsSolid())
                break;
            else
                obj->x+=xChange;
        }
    } else {
        for (int stepX = 0; stepX < distX; stepX++) {
            if (world->tiles[obj->x+xChange][obj->y]->IsSolid())
                break;
            else
                obj->x+=xChange;
        }
        for (int stepY = 0; stepY < distY; stepY++) {
            if (world->tiles[obj->x][obj->y+yChange]->IsSolid())
                break;
            else
                obj->y+=yChange;
        }
    }

    SDL_Log("Attempting step %i, %i with signs %i, %i\tNew Position: %i, %i", dx, dy, distX, distY, obj->x, obj->y);

    return;
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
    Map level = Map();
    level.WallRectangle(MapWidth,MapHeight);
    level.FillRectangle(30,1,38,13,2);
    level.FillRectangle(10,4,10,10,3);

    Entity* player = new Entity(20, 7, 0);

    //Entity* entities[MaxEntities];
    //entities[0] = new Entity(20, 7, 0);

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
                Move_QueueClear = true;
        }
        



        // Game Logic
        GameTick+=DeltaTime*TickRate;
        if (GameTick >= 1) {
            ticks++;
            window.LogTick();
            //SDL_Log("Game tick %i, DT %f", ticks, DeltaTime);

            if (MoveUp)
                Movement_ShiftEntity(&level, player, 0, 1);
            else if (MoveDown)
                Movement_ShiftEntity(&level, player, 0, -1);
            else if (MoveRight)
                Movement_ShiftEntity(&level, player, 1, 0);
            else if (MoveLeft)
                Movement_ShiftEntity(&level, player, -1, 0);

            if (Move_QueueClear)
                Movement_Clear();

            GameTick = 0;
        }





        // Render
        SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
        SDL_RenderClear(window.canvas);
        window.UpdateSize();
        window.TickDeltaTime(DeltaTime);
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                window.DrawTile(x,y,level.tiles[x][y]->GetTileID());
            }
        }
        window.DrawEntity(player->x, player->y, player->GetID());
        window.DrawDialogueBox();
        SDL_RenderPresent(window.canvas);
    }

    window.Close();
    level.Free();
    delete player;

    return 0;
}