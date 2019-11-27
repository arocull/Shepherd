#pragma once

#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "charmap.h"
#include "mathutil.h"

// Render Window - An individual window that renders things.
class RenderWindow {
    public:
        RenderWindow();
        RenderWindow(int viewportX, int viewportY, const char* windowName);
        
        SDL_Window *window;
        SDL_Renderer *canvas;

    private:
        bool initialized = false;
        //Full X size of window
        int x;
        //Full Y size of window
        int y;
        
        //How many pixels per tile side length there are (same for X and Y)
        int tileRes;
        //Width of viewport inside the window
        int innerWidth;
        //Height of viewport inside the window
        int innerHeight;
        //X Offset of viewport inside the window
        int offsetX;
        //Y Offset of viewport inside the window
        int offsetY;

        bool fullscreen = false;
        int windowedX;
        int windowedY;

        //How much time has passed since the window was created
        float time = 0;
        //How many game ticks have passed since the window was created
        int ticks = 0;


        //Y Size of the dialogue box
        int dialogueBoxY;
        int dialogueBoxLineHeight;
        bool dialogueBoxVisible = true;
        char* dialogueText;
        int dialogueTicksLeft = 0;


        SDL_Surface* TEXTURESURFACE_tree;
        SDL_Texture* TEXTURE_tree;

        SDL_Surface* TEXTURESURFACE_rock;
        SDL_Texture* TEXTURE_rock;

        SDL_Surface* TEXTURESURFACE_sheep;
        SDL_Texture* TEXTURE_sheep;

        SDL_Surface* TEXTURESURFACE_shepherd;
        SDL_Texture* TEXTURE_shepherd;

        SDL_Surface* TEXTURESURFACE_alphabet;
        SDL_Texture* TEXTURE_alphabet;

    public:
        void FillViewportBackground(int r, int g, int b);
        void DrawTile(int tileX, int tileY, int tileID);
        void DrawEntity(int posX, int posY, int id, bool flip, int animation);

        void DrawLetter(int posX, int posY, int sizeX, int sizeY, char letter);
        int WriteText(int leftX, int topY, int rightX, int bottomY, char* text, int start = 0, int end = -1);
        
        void SetDialogueText(char* newText, int ticks = 50);
        char* GetDialogueText();
        void DrawDialogueBox();
        

        bool IsInitialized();
        void ToggleFullscreen();

        void UpdateSize();
        void TickDeltaTime(float DeltaTime);
        void LogTick();

        void Close();
};