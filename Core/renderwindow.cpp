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

        TEXTURESURFACE_tree = SDL_LoadBMP("Textures/Tree.bmp");
        if (TEXTURESURFACE_tree) {
            TEXTURE_tree = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_tree);
        }

        TEXTURESURFACE_sheep = SDL_LoadBMP("Textures/Sheep.bmp");
        if (TEXTURESURFACE_sheep) {
            TEXTURE_sheep = SDL_CreateTextureFromSurface(canvas, TEXTURESURFACE_sheep);
        }
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

    SDL_FreeSurface(TEXTURESURFACE_sheep);
    SDL_DestroyTexture(TEXTURE_sheep);

    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
}



void RenderWindow::FillViewportBackground(int r, int g, int b) {
    SDL_Rect box;
    box.x = offsetX;
    box.y = offsetY;
    box.w = tileRes*MapWidth;
    box.h = tileRes*MapHeight;

    SDL_SetRenderDrawColor(canvas, r, g, b, 0);
    SDL_RenderFillRect(canvas, &box);
    return;
}
void RenderWindow::DrawTile(int tileX, int tileY, int tileID) {
    if (tileID == 0) return;

    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = tileX*tileRes + offsetX;
    tile.y = tileY*tileRes + offsetY;

    if (tileID == 1) {      // Wall
        //if ((time - round(time)) <= .5 && )
        if ((tileX % 2 == 1 && tileY % 2 == 0) || (tileX % 2 == 0 && tileY % 2 == 1))
            SDL_SetRenderDrawColor(canvas, 215, 215, 215, 0);
        else
            SDL_SetRenderDrawColor(canvas, 235, 235, 235, 0);
        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 2) {   //Water
        SDL_SetRenderDrawColor(canvas, 0, 30, 200 + (int) (sin(time + (sqrt(pow(tileX,2) + pow(tileY,2)))) * 20), 0);

        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 3) {   //Magma
        float z = sqrt(pow(tileX,2) + pow(tileY,2));
        SDL_SetRenderDrawColor(canvas, 220 + (int) (sin(time/2 + z) * 30), 80 + (int) (sin(time/3 + z) * 20), 20, 0);
        
        SDL_RenderFillRect(canvas, &tile);
    } else if (tileID == 4) {   //Tree
        SDL_RenderCopy(canvas, TEXTURE_tree, NULL, &tile);
    }
}
void RenderWindow::DrawEntity(int posX, int posY, int id, bool flip) {
    SDL_Rect tile;
    tile.w = tileRes;
    tile.h = tileRes;
    tile.x = posX*tileRes + offsetX;
    tile.y = posY*tileRes + offsetY;

    /*SDL_Point center;
    center.x = tile.x+tile.w/2;
    center.y = tile.y+tile.h/2;*/

    SDL_RendererFlip flipStyle = SDL_FLIP_NONE;
    if (flip)
        flipStyle = SDL_FLIP_HORIZONTAL;

    //SDL_RenderCopy(canvas, TEXTURE_sheep, NULL, &tile);

    double angle = 0.0;
    if (id == 1 || id == 2) {  // Player or Sheep
        int step = ticks/2 % 4;
        if (step == 2)
            angle = -3.0;
        else if (step == 1 || step == 3)
            angle = 0.0;
        else
            angle = 3.0;

        SDL_RenderCopyEx(canvas, TEXTURE_sheep, NULL, &tile, angle, NULL, flipStyle);
    } else if (id == 3) {   // Fireball
        int sha = 20 * (float) (sin(time) + 1);
        SDL_SetRenderDrawColor(canvas, 220 + sha, 100 + sha, 50, 0);

        tile.x += tile.w*.25;
        tile.y += tile.h*.25;
        tile.w *= .5;
        tile.h *= .5;

        SDL_RenderFillRect(canvas, &tile);
    }    
}
void RenderWindow::DrawDialogueBox() {
    SDL_SetRenderDrawColor(canvas, 120, 120, 120, 0);
    SDL_RenderDrawLine(canvas,0,offsetY,x,offsetY);
    SDL_RenderDrawLine(canvas,0,innerHeight+offsetY,x,innerHeight+offsetY);
}