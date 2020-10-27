#pragma once

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "charmap.h"
#include "mathutil.h"

#include "Core/enums.h"

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
        float time = 0.0f;
        //How many game ticks have passed since the window was created
        int ticks = 0;
        // Value between 0 and 1, current time progression through tick (used for interpolation)
        float tickAlpha = 0.0f;


        //Y Size of the dialogue box
        int dialogueBoxY;
        int dialogueBoxLineHeight;
        bool dialogueBoxVisible = true;
        char* dialogueText = NULL;
        int dialogueTicksLeft = 0;

        int statusBarY;
        bool statusBarVisible = false;

        SDL_Surface* TEXTURESURFACE_PROGRAM_loadscreen;
        SDL_Texture* TEXTURE_PROGRAM_loadscreen;
        SDL_Surface* TEXTURESURFACE_PROGRAM_icon;

        SDL_Surface* TEXTURESURFACE_tree;
        SDL_Texture* TEXTURE_tree;

        SDL_Surface* TEXTURESURFACE_rock;
        SDL_Texture* TEXTURE_rock;

        SDL_Surface* TEXTURESURFACE_pillar;
        SDL_Texture* TEXTURE_pillar;

        SDL_Surface* TEXTURESURFACE_vines;
        SDL_Texture* TEXTURE_vines;

        SDL_Surface* TEXTURESURFACE_sunTile;
        SDL_Texture* TEXTURE_sunTile;

        SDL_Surface* TEXTURESURFACE_crate;
        SDL_Texture* TEXTURE_crate;

        SDL_Surface* TEXTURESURFACE_torch;
        SDL_Texture* TEXTURE_torch;

        SDL_Surface* TEXTURESURFACE_lever;
        SDL_Texture* TEXTURE_lever;

        SDL_Surface* TEXTURESURFACE_sheep;
        SDL_Texture* TEXTURE_sheep;

        SDL_Surface* TEXTURESURFACE_wolf;
        SDL_Texture* TEXTURE_wolf;

        SDL_Surface* TEXTURESURFACE_shepherd;
        SDL_Texture* TEXTURE_shepherd;

        SDL_Surface* TEXTURESURFACE_alphabet;
        SDL_Texture* TEXTURE_alphabet;

    public:
        void FillViewportBackground(int r, int g, int b);
        void DrawTile(int tileX, int tileY, int tileID, int tilingIndex);
        void DrawEntity(int posX, int posY, int lastX, int lastY, int id, bool flip, int animation, int metadata);
        void DrawParticle(float posX, float posY, int id, float percentage);
        
        void SetDialogueText(const char* newText, int ticks = 50);
        char* GetDialogueText();
        void DrawDialogueBox(char* text = nullptr);

        void DrawStatusBar(int HP, bool PuzzleCompleted);
        void ToggleStatusBar(bool toggle);

        void DrawMenuBackground();
        void DrawMenu(int menuSize, char** menuText, int itemSelected, bool shrunk = false);
        void LoadScreen();

        bool IsInitialized();
        void ToggleFullscreen(bool setFullscreen);
        void ToggleFullscreen();
        bool InFullscreen();

        void UpdateSize();
        void TickDeltaTime(float DeltaTime);
        void LogTick();

        void Close();
    
    private:
        void DrawLetter(int posX, int posY, int sizeX, int sizeY, char letter);
        int WriteText(int leftX, int topY, int rightX, int bottomY, char* text, int start = 0, int end = -2);
};