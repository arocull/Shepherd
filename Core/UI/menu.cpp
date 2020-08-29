#include "Core/UI/menu.h"

Menu::Menu(MenuID menu, int numItems) {
    id = menu;
    numOptions = numItems;
    optionIndex = 0;

    optionNames = (char**) calloc(numItems, sizeof(char*));
    optionDesc = (char**) calloc(numItems, sizeof(char*));

    active = true;    
}

int Menu::getNumOptions() {
    return numOptions;
}

MenuID Menu::getMenuID() {
    return id;
}

// Frees allocated memory for optionNames and optionDesc
// If freeStrings is true, it attempts to free the strings inside of the arrays as well
void Menu::Free(bool freeStrings) {
    active = false;

    if (freeStrings) {
        for (int i = 0; i < numOptions; i++) {
            free(optionNames[i]);
            free(optionDesc[i]);
        }
    }

    free(optionNames);
    free(optionDesc);
}