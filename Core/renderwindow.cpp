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
    
    if (initialized)
        SDL_SetWindowTitle(window, windowName);
}


bool RenderWindow::IsInitialized(){
    return initialized;
}
void RenderWindow::ToggleFullscreen() {
    if (!fullscreen) {
        windowedX = x;
        windowedY = y;
        SDL_SetWindowBordered(window, SDL_FALSE);
        SDL_SetWindowFullscreen(window, SDL_TRUE);
        fullscreen = true;
    } else {
        SDL_SetWindowBordered(window, SDL_TRUE);
        SDL_SetWindowSize(window, windowedX, windowedY);
        fullscreen = false;
    }
}
void RenderWindow::UpdateSize() {
    tileRes = fmax(x/MapWidth, y/MapHeight);
}



void RenderWindow::Close() {
    if (!initialized) return;
    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
}



void RenderWindow::DrawTile(int tileX, int tileY, int tileID) {
    if (tileID == 0) return;
    int resX = x/40;
    int resY = y/15;

    SDL_Rect tile;
    tile.w = resX;
    tile.h = resY;
    tile.x = tileX*resX;
    tile.y = tileY*resY;

    if (tileID == 1) {
        SDL_SetRenderDrawColor(canvas, 0x10, 0x10, 0x10, 0x00);
        SDL_RenderDrawRect(canvas, &tile);
    }
}
void RenderWindow::DrawDialogueBox() {

}