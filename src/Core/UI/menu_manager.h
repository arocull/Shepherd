#pragma once

#include <string.h>

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
        void EnterSaveLoading();

        void ToggleFullscreen(bool inFullscreen);

        bool inSubmenu();
        // Frees menu data so MenuManager can be deleted
        void Free();
        // Frees specifically the load-a-save screen, safe for multiple calls
        void FreeSaveLoading();

    private:
        Menu* mapMenu;
        Menu* scrollsMenu;
        Menu* settingsMenu;
        Menu* loadingSave;

        void ClampOptionIndex();
};