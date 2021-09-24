#include "Core/UI/menu_manager.h"

MenuManager::MenuManager() {
    pauseMenu = new Menu(MenuID::EM_Pause, 5); // Fill out pause menu (cast to char array to prevent compiler auto-conversion to string)
    pauseMenu->optionNames[0] = (char*) "Resume";
    pauseMenu->optionNames[1] = (char*) "Map";
    pauseMenu->optionNames[2] = (char*) "Scrolls";
    pauseMenu->optionNames[3] = (char*) "Settings";
    pauseMenu->optionNames[4] = (char*) "Close";
    pauseMenu->optionDesc[0] = (char*) "Resumes the game";
    pauseMenu->optionDesc[1] = (char*) "Displays a map of the explored areas";
    pauseMenu->optionDesc[2] = (char*) "Any hidden texts you have found";
    pauseMenu->optionDesc[3] = (char*) "Tailor your game experience with things like volume and fullscreen";
    pauseMenu->optionDesc[4] = (char*) "Closes the game";

    mapMenu = nullptr;

    scrollsMenu = nullptr;

    settingsMenu = new Menu(MenuID::EM_Settings, 3);
    settingsMenu->optionNames[0] = (char*) "Volume";
    settingsMenu->optionNames[1] = (char*) "Windowed";
    settingsMenu->optionNames[2] = (char*) "Reset Save";
    settingsMenu->optionDesc[0] = (char*) "Change volume of music and sound effects.";
    settingsMenu->optionDesc[1] = (char*) "Toggles between fullscreen and windowed mode--also toggleable with F11.";
    settingsMenu->optionDesc[2] = (char*) "Resets all save data and forcibly closes the game.";

    activeMenu = pauseMenu;

    loadingSave = new Menu(MenuID::EM_SaveLoading, 2);
    loadingSave->optionNames[0] = (char*) "Load Save";
    loadingSave->optionNames[1] = (char*) "New Game";
    loadingSave->optionDesc[0] = (char*) "Loads the currently saved game. (NOTE THAT THIS IS CURRENTLY EXPERIMENTAL. IT CAN AND WILL BREAK.)";
    loadingSave->optionDesc[1] = (char*) "Starts a new game and overrides your existing save.";
}


// Initializes the scrolls menu based off the total number of scrolls loaded 
void MenuManager::InitScrolls(int totalScrolls) {
    if (scrollsMenu) return; // Do not allow re-initialization
    scrollsMenu = new Menu(MenuID::EM_Scrolls, totalScrolls);

    // Fill scrolls with blanks
    for (int i = 0; i < totalScrolls; i++) {
        scrollsMenu->optionNames[i] = (char*) "??????";
        scrollsMenu->optionDesc[i] = (char*) "Scroll not discovered.";
    }
}
// Adds a scroll into the scroll library
void MenuManager::UnlockScroll(int scrollIndex, char* name, char* desc) {
    scrollsMenu->optionNames[scrollIndex] = strdup(name);
    scrollsMenu->optionDesc[scrollIndex] = strdup(desc);
}


// Goes up one layer of menu--currently jumps to main pause menu
void MenuManager::Back() {
    if (inSubmenu()) {
        activeMenu = pauseMenu;
    }
}



void MenuManager::OptionUp() {
    activeMenu->optionIndex--;
    ClampOptionIndex();
}
void MenuManager::OptionDown() {
    activeMenu->optionIndex++;
    ClampOptionIndex();
}
int MenuManager::OptionIndex() {
    return activeMenu->optionIndex;
}
void MenuManager::ClampOptionIndex() {
    if (activeMenu->optionIndex >= activeMenu->getNumOptions()) activeMenu->optionIndex = 0; // Wrap to top
    else if (activeMenu->optionIndex < 0) activeMenu->optionIndex = activeMenu->getNumOptions() - 1; // Wrap to bottom
}



// TODO: Streamline these with Enums + array indices?
void MenuManager::EnterMenuMap() {
    activeMenu = mapMenu;
}
void MenuManager::EnterMenuScrolls() {
    activeMenu = scrollsMenu;
}
void MenuManager::EnterMenuSettings() {
    activeMenu = settingsMenu;
}
void MenuManager::EnterSaveLoading() {
    activeMenu = loadingSave;
}



void MenuManager::ToggleFullscreen(bool inFullscreen) {
    if (inFullscreen) settingsMenu->optionNames[1] = (char*) "Fullscreen";
    else settingsMenu->optionNames[1] = (char*) "Windowed";
}



bool MenuManager::inSubmenu() {
    return (activeMenu != pauseMenu && activeMenu != loadingSave);
}

void MenuManager::Free() {
    FreeSaveLoading();
    activeMenu = nullptr;

    if (scrollsMenu) {
        scrollsMenu->Free();
        delete scrollsMenu;
        scrollsMenu = nullptr;
    }

    settingsMenu->Free();
    delete settingsMenu;
    pauseMenu->Free();
    delete pauseMenu;
}
void MenuManager::FreeSaveLoading() {
    if (loadingSave) {
        loadingSave->Free();
        delete loadingSave;
        loadingSave = nullptr;
    }
    activeMenu = pauseMenu;
}