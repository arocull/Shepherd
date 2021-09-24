#pragma once

#include <SDL2/SDL.h>

#include "Core/config.h"
#include "Core/gamedata.h"

#include "Core/renderwindow.h"
#include "Audio/sound_service.h"
#include "Core/UI/menu_manager.h"
#include "Core/Input/controller.h"
#include "Entities/AI/ai_manager.h"

#include "Map/save.h"
#include "Map/map_loading.h"
#include "Entities/entity_management.h"
#include "Triggers/triggers.h"

class Game {
    public:
        Game(RenderWindow* gameWindow);
        ~Game();

        GameData* data;
        bool paused;
        bool close;
    
    private:
        RenderWindow* window;
        SoundService* audio;
        MenuManager* menus;
        Controller* controller;
        AIManager* ai;

        int maxMapID = 0;
        bool tickedThisFrame = false;

        int loadedMapX = 0;
        int loadedMapY = 0;
        float tickTimer = 0;
    
    public:
        // Process Input (window-specific functions happen here)
        struct InputAction* ProcessInput(SDL_Event* event);
        
        void Tick();
        // Ticks general utilities like audio and particles, returns true if a tick should occur
        bool Step(float deltaTime);
        void Draw(float deltaTime);

        void DrawPauseMenu(float deltaTime);
        void SaveMenuOpen();
        void SaveMenuClose();

        // Creates a new game, loading default settings
        void LoadGameDefaults();
        // Attempts to load a saved game, returns true if successful, false otherwise
        bool LoadGame();
        // Attempts to save the game, good for checkpoints
        void SaveGame();

    private:
        // Loads scroll data from the currently loaded maps
        void LoadScrolls();

        // Ticks player movement once
        void MovePlayer();
};