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

        float time = 0;
        int ticks = 0;

    public:
        void DrawTile(int tileX, int tileY, int tileID);
        void DrawEntity(int posX, int posY, int id);
        void DrawDialogueBox();

        bool IsInitialized();
        void ToggleFullscreen();

        void UpdateSize();
        void TickDeltaTime(float DeltaTime);
        void LogTick();

        void Close();
};