#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "renderwindow.h"

RenderWindow::RenderWindow() {
    RenderWindow(400, 400, "SDL Render Window");
}
RenderWindow::RenderWindow(int viewportX, int viewportY, const char* windowName) {
    x = viewportX;
    y = viewportY;

    if (SDL_CreateWindowAndRenderer(viewportX, viewportY, SDL_WINDOW_RESIZABLE, &window, &canvas)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window and renderer: %s", SDL_GetError());
        initialized = false;
    } else
        initialized = true;
    
    if (initialized) {
        UpdateSize();
        SDL_SetWindowTitle(window, windowName);
    }
}


bool RenderWindow::IsInitialized(){
    return initialized;
}
void RenderWindow::ToggleFullscreen() {
    if (!fullscreen) {
        windowedX = x;
        windowedY = y;
        
        /*SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &DM);
        int DisplayWidth = DM.w;
        int DisplayHeight = DM.h;

        //SDL_SetWindowSize(window, DisplayWidth, DisplayHeight);
        //SDL_SetWindowBordered(window, SDL_FALSE);*/
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); //Enforce borderless fullscreen without screwing up resolution
        fullscreen = true;
    } else {
        SDL_SetWindowFullscreen(window, 0); //Exit fullscreen
        //SDL_SetWindowBordered(window, SDL_TRUE);
        SDL_SetWindowSize(window, windowedX, windowedY);
        fullscreen = false;
    }
}
void RenderWindow::UpdateSize() {
    SDL_GetWindowSize(window, &x, &y);
    tileRes = fmin(x/MapWidth, y/MapHeight);
    offsetX = (x-MapWidth*tileRes)/2;
    offsetY = (y-MapHeight*tileRes)/2;
    innerWidth = tileRes*MapWidth;
    innerHeight = tileRes*MapHeight;
}
void RenderWindow::TickDeltaTime(float DeltaTime) {
    time+=DeltaTime;
}
void RenderWindow::LogTick() {
    ticks++;
}



void RenderWindow::Close() {
    if (!initialized) return;
    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
}



void RenderWindow::DrawTile(int tileX, int tileY, int tileID) {
    if (tileID == 0) return;

    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = tileX*tileRes + offsetX;
    tile.y = tileY*tileRes + offsetY;

    if (tileID == 1) {
        //if ((time - round(time)) <= .5 && )
        if ((tileX % 2 == 1 && tileY % 2 == 0) || (tileX % 2 == 0 && tileY % 2 == 1))
            SDL_SetRenderDrawColor(canvas, 215, 215, 215, 0);
        else
            SDL_SetRenderDrawColor(canvas, 235, 235, 235, 0);
        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 2) {
        SDL_SetRenderDrawColor(canvas, 0, 30, 200 + (int) (sin(time + (sqrt(pow(tileX,2) + pow(tileY,2)))) * 20), 0);

        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 3) {
        SDL_SetRenderDrawColor(canvas, 50, 200, 75, 0);
        SDL_RenderDrawLine(canvas, tileRes/2 + tile.x, tile.y,         tile.x,         tile.y+tileRes*.9);
        SDL_RenderDrawLine(canvas, tileRes/2 + tile.x, tile.y,         tile.x+tileRes, tile.y+tileRes*.9);
        SDL_RenderDrawLine(canvas, tile.x,             tile.y+tileRes*.9, tile.x+tileRes,         tile.y+tileRes*.9);
        
        tile.w-=3*tileRes/4;
        tile.h-=7*tileRes/8;
        tile.x+=7*tileRes/16;
        tile.y+=7*tileRes/8;

        SDL_SetRenderDrawColor(canvas, 150, 125, 25, 0);
        SDL_RenderFillRect(canvas, &tile);
    }
}
void RenderWindow::DrawEntity(int posX, int posY, int id) {
    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = posX*tileRes + offsetX;
    tile.y = posY*tileRes + offsetY;

    SDL_SetRenderDrawColor(canvas, 10, 255, 120, 0);
    SDL_RenderFillRect(canvas, &tile);
}
void RenderWindow::DrawDialogueBox() {
    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    SDL_RenderDrawLine(canvas,0,offsetY,x,offsetY);
    SDL_RenderDrawLine(canvas,0,innerHeight+offsetY,x,innerHeight+offsetY);
}