#pragma once

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
    
    private:
        RenderWindow* window;
        SoundService* audio;
        MenuManager* menus;
        Controller* controller;
        AIManager* ai;

        int maxMapID;
        bool tickedThisFrame;
    
    public:
        void Tick();
        void Step(float deltaTime);
        void Draw(float deltaTime);

    private:
        // Loads scroll data from the currently loaded maps
        void LoadScrolls();

        // Creates a new game, loading default settings
        void NewGame();
};