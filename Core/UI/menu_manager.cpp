#include "Core/UI/menu_manager.h"

MenuManager::MenuManager() {
    pauseMenu = new Menu(5); // Fill out pause menu (cast to char array to prevent compiler auto-conversion to string)
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

    mapMenu;

    scrollsMenu;
    scrollsInitialized = false;

    settingsMenu = new Menu(3);
    settingsMenu->optionNames[0] = (char*) "Volume";
    settingsMenu->optionNames[1] = (char*) "Fullscreen";
    settingsMenu->optionNames[2] = (char*) "Reset Save";
    settingsMenu->optionDesc[0] = (char*) "This is a submenu test. (Not currently functional.)";
    settingsMenu->optionDesc[1] = (char*) "Select to toggle between fullscreen and windowed mode. Also toggleable with F11. (Not currently functional.)";
    settingsMenu->optionDesc[2] = (char*) "Resets all save data. Forcecloses game. (Not currently functional.)";

    activeMenu = pauseMenu;
}



void MenuManager::InitScrolls(int totalScrolls) {
    if (scrollsInitialized) return; // Do not allow re-initialization
    scrollsMenu = new Menu(totalScrolls);

    // Fill scrolls with blanks
    for (int i = 0; i < totalScrolls; i++) {
        scrollsMenu->optionNames[i] = (char*) "??????";
        scrollsMenu->optionDesc[i] = (char*) "Scroll undiscovered.";
    }
}
void MenuManager::UnlockScroll(int scrollIndex, char* name, char* desc) {
    scrollsMenu->optionNames[scrollIndex] = name;
    scrollsMenu->optionDesc[scrollIndex] = desc;
}


// Goes up one layer of menu--currently jumps to main pause menu
void MenuManager::Back() {
    activeMenu = pauseMenu;
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



void MenuManager::EnterMenuMap() {
    activeMenu = mapMenu;
}
void MenuManager::EnterMenuScrolls() {
    activeMenu = scrollsMenu;
}
void MenuManager::EnterMenuSettings() {
    activeMenu = settingsMenu;
}



void MenuManager::ToggleFullscreen(bool inFullscreen) {
    if (inFullscreen) settingsMenu->optionNames[1] = (char*) "Windowed";
    else if (inFullscreen) settingsMenu->optionNames[1] = (char*) "Fullscreen";
}



bool MenuManager::inSubmenu() {
    return (activeMenu != pauseMenu);
}

void MenuManager::Free() {
    activeMenu = nullptr;

    pauseMenu->Free();
    if (scrollsInitialized) scrollsMenu->Free();
    settingsMenu->Free();

    delete pauseMenu;
    if (scrollsInitialized) delete scrollsMenu;
    delete settingsMenu;

    scrollsInitialized = false;
}