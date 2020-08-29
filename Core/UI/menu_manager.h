#pragma once

#include "Core/enums.h"
#include "Core/UI/menu.h"

class MenuManager {
    public:
        MenuManager();

        Menu* pauseMenu;
        Menu* activeMenu;

        void InitScrolls(int totalScrolls);
        void UnlockScroll(int scrollIndex, char* name, char* desc);

        void Back();

        void OptionUp();
        void OptionDown();
        int OptionIndex();

        void EnterMenuMap();
        void EnterMenuScrolls();
        void EnterMenuSettings();

        void ToggleFullscreen(bool inFullscreen);

        bool inSubmenu();
        void Free();

    private:
        Menu* mapMenu;
        Menu* scrollsMenu;
        Menu* settingsMenu;

        bool scrollsInitialized;

        void ClampOptionIndex();
};