#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"

class RenderWindow {
    public:
        RenderWindow();
        RenderWindow(int viewportX, int viewportY, const char* windowName);
        
        SDL_Window *window;
        SDL_Renderer *canvas;

    private:
        bool initialized = false;
        int x;
        int y;
        
        int tileRes;
        int innerWidth;
        int innerHeight;
        int offsetX;
        int offsetY;

        bool fullscreen = false;
        int windowedX;
        int windowedY;

    public:
        void DrawTile(int tileX, int tileY, int tileID);
        void DrawDialogueBox();

        bool IsInitialized();
        void ToggleFullscreen();

        void UpdateSize();

        void Close();
};