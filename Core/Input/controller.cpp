#include "Core/Input/controller.h"

Controller::Controller(MenuManager* menuManager, SoundService* soundService) {
    menus = menuManager;
    sounds = soundService;
}

// Processes the given input and applies it to the given InputAction struct
// TODO: Pair inputs with specific controllers - https://gamedev.stackexchange.com/questions/152350/how-can-sdl-tell-what-xbox360-controller-is-what-player
// SDL Wiki reference: https://wiki.libsdl.org/SDL_Event
void Controller::ProcessInput(SDL_Event* input, InputAction* action, bool paused, bool inFullscreen) {
    action->close = false;
    action->pause = paused;
    action->fullscreen = inFullscreen;


    switch (input->type) {
        case SDL_KEYDOWN:
            KeyDown(input->key.keysym.sym, action); break;
        case SDL_KEYUP:
            KeyUp(input->key.keysym.sym); break;
    }
}


void Controller::KeyDown(SDL_Keycode key, InputAction* action) {
    switch (key) {
        // Settings
        case SDLK_ESCAPE: // Open / Close pause menu, or back out of submenus
            if (!KeyEscapeDown) {
                KeyEscapeDown = true;
                if (menus->inSubmenu()) menus->Back();
                else action->pause = !action->pause;

                MoveFireballQueued = false;
                Movement_Clear();
            }
            break;
        case SDLK_F11: // Toggle fullscreen
            action->fullscreen = !action->fullscreen;
            menus->ToggleFullscreen(action->fullscreen);
            break;
        #ifdef DEBUG_MODE   // In Debug Mode, we can hold shift to accelerate the game
            case SDLK_LSHIFT: case SDLK_RSHIFT:
                accelerate = true;
                break;
        #endif
        
        // Movement (doubles between WASD and arrow keys)
        case SDLK_w: case SDLK_UP:
            Movement_Clear();
            MoveUp = true;
            break;
        case SDLK_s: case SDLK_DOWN:
            Movement_Clear();
            MoveDown = true;
            break;
        case SDLK_d: case SDLK_RIGHT:
            Movement_Clear();
            MoveRight = true;
            break;
        case SDLK_a: case SDLK_LEFT:
            Movement_Clear();
            MoveLeft = true;
            break;
        case SDLK_SPACE: case SDLK_RETURN: case SDLK_RETURN2: // Sling fireball or use menu
            MoveFireballQueued = true;
            break; 
    }

    if (action->pause) { // If game is paused, navigate menus
        if (MoveLeft) { // Back out of submenus if left was pressed (same functionality as escape without unpausing)
            menus->Back();
        }

        if (MoveUp) menus->OptionUp(); // Move up in menu
        else if (MoveDown) menus->OptionDown(); // Move down in menu

        if (menus->inSubmenu()) { // If in submenu, manipulate index
            if (MoveRight || MoveFireballQueued) { // When enter / right / spacebar
                // Menu functionality
                switch (menus->activeMenu->getMenuID()) {

                    case MenuID::EM_Settings: // Settings menu functionality
                        switch (menus->activeMenu->optionIndex) {
                            case 0: // Volume control
                                sounds->ConfiguredVolumeDecrease();
                                break;
                            case 1: // Fullscreen / Windowed
                                action->fullscreen = !action->fullscreen; // Flip fullscreen on / off
                                break;
                            case 2: // Reset save data (currently no data to reset, but close window)
                                action->close = true;
                                break;
                        }
                        break;
                }

                sounds->PlaySound("Audio/Resources/Click1.wav");
            }
        } else {
            if (MoveRight || MoveFireballQueued) { // When enter / right / spacebar is pressed
                switch (menus->OptionIndex()) {
                    case 0: action->pause = false; break; // Resume
                    // case 1: activeMenu // Display map
                    case 2: menus->EnterMenuScrolls(); break;
                    case 3: menus->EnterMenuSettings(); break;
                    case 4: action->close = true; break;
                }

                sounds->PlaySound("Audio/Resources/Click1.wav");
            }
        }

        MoveFireballQueued = false;
        Movement_Clear(); // Clear input so it is not carried back into game
    }
}
void Controller::KeyUp(SDL_Keycode key) {
    switch (key) {
        case SDLK_w: case SDLK_UP:
        case SDLK_s: case SDLK_DOWN:
        case SDLK_d: case SDLK_RIGHT:
        case SDLK_a: case SDLK_LEFT:
            Move_QueueClear = true; // Letting go of any movement key qeues a move clear (does not clear immeadietly to prevent 'sticky' controls)
            break;
        case SDLK_ESCAPE:
            KeyEscapeDown = false;
            break;
        #ifdef DEBUG_MODE // Stop game acceleration (DEBUG MODE only)
            case SDLK_LSHIFT: case SDLK_RSHIFT:
                accelerate = false;
                break;
        #endif
    }
}