#pragma once

#include "Core/config.h"

#include <SDL2/SDL.h>

#include "Audio/sound_service.h"
#include "Core/UI/menu_manager.h"
#include "Core/Input/input_action.h"

#include "Entities/movement.h"

class Controller {
    public:
        Controller(MenuManager* menuManager, SoundService* soundService);

        void ProcessInput(SDL_Event* input, InputAction* action, bool paused, bool inFullscreen);

        #ifdef DEBUG_MODE
            bool accelerate = false;
        #endif
    
    private:
        MenuManager* menus;
        SoundService* sounds;

        bool KeyEscapeDown = false;
        bool KeyFullscreenDown = false;

        void KeyDown(SDL_Keycode key, InputAction* action);
        void KeyUp(SDL_Keycode key);
    
    public:
        // Input State //
};