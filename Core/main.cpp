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

    int distX = abs(dx);
    int distY = abs(dy);

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

    //SDL_Log("Attempting step %i, %i with signs %i, %i\tNew Position: %i, %i", dx, dy, distX, distY, obj->x, obj->y);

    return;
}
void Movement_ShiftPlayer(Map* world, Entity* obj, int dx, int dy, int* worldX, int* worldY) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;


    if (desiredX >= MapWidth) {
        *worldX = *worldX+1;
        SDL_Log("Shifting World X Coordinate up 1");
        obj->x = 0;
    } else if (desiredX < 0) {
        *worldX = *worldX-1;
        SDL_Log("Shifting World X Coordinate down 1");
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !world->tiles[desiredX][obj->y]->IsSolid())
        obj->x = desiredX;
    
    if (desiredY >= MapHeight) {
        *worldY = *worldY+1;
        SDL_Log("Shifting World Y Coordinate up 1");
        obj->y = 0;
    } else if (desiredY < 0) {
        *worldY = *worldY-1;
        SDL_Log("Shifting World Y Coordinate down 1");
        obj->y = MapHeight-1;
    } else if (desiredY < MapHeight && desiredY >= 0 && !world->tiles[obj->x][desiredY]->IsSolid())
        obj->y = desiredY;


    //SDL_Log("Attempting step %i, %i with signs %i, %i\tNew Position: %i, %i", dx, dy, distX, distY, obj->x, obj->y);
    SDL_Log("Finished player movement.");
    return;
}



Map* LoadLevel(Map* world[WorldWidth][WorldHeight], int worldX, int worldY) {
    SDL_Log("Attempting load of World Coordinate %i, %i", worldX, worldY);

    Map* newLevel = world[worldX][worldY];

    SDL_Log("Loaded World Coordinate %i, %i", worldX, worldY);

    return newLevel;
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


    Map* world[WorldWidth][WorldHeight];
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++)
            world[x][y] = new Map();
    }
    int worldX = 1;
    int worldY = 1;
    int currentWorldX = worldX;
    int currentWorldY = worldY;

    Map* currentLevel = LoadLevel(world, worldX, worldY);
    //currentLevel->WallRectangle(MapWidth,MapHeight);
    currentLevel->FillRectangle(5,5,10,10, 2);

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
                Movement_ShiftPlayer(currentLevel, player, 0, 1, &worldX, &worldY);
            else if (MoveDown)
                Movement_ShiftPlayer(currentLevel, player, 0, -1, &worldX, &worldY);
            else if (MoveRight)
                Movement_ShiftPlayer(currentLevel, player, 1, 0, &worldX, &worldY);
            else if (MoveLeft)
                Movement_ShiftPlayer(currentLevel, player, -1, 0, &worldX, &worldY);

            if (Move_QueueClear)
                Movement_Clear();


            
            if (worldX >= WorldWidth)
                worldX = WorldWidth-1;
            else if (worldX < 0)
                worldX = 0;
            if (worldY >= WorldHeight)
                worldY = WorldHeight-1;
            else if (worldY < 0)
                worldY = 0;
            SDL_Log("Current World Coordinate is %i, %i", worldX, worldY);
            if (worldX != currentWorldX || worldY != currentWorldY) {
                currentLevel = LoadLevel(world, worldX, worldY);
            }

            GameTick = 0;
        }





        // Render
        SDL_SetRenderDrawColor(window.canvas, 0, 0, 0, 0);
        SDL_RenderClear(window.canvas);
        window.UpdateSize();
        window.TickDeltaTime(DeltaTime);
        for (int x = 0; x < MapWidth; x++) {
            for (int y = 0; y < MapHeight; y++) {
                window.DrawTile(x,y,currentLevel->tiles[x][y]->GetTileID());
            }
        }
        window.DrawEntity(player->x, player->y, player->GetID());
        window.DrawDialogueBox();
        SDL_RenderPresent(window.canvas);
    }

    window.Close();
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            world[x][y]->Free();
            delete world[x][y];
        }
    }
    delete player;

    return 0;
}