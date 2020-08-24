#pragma once

#include <cstdlib>

#include "Core/enums.h"

// Menu - A UI class that holds various data for displaying such as string lists
class Menu {
    public:
        Menu(MenuID menu, int numItems);
    
    private:
        int numOptions;
        MenuID id;

    public:
        char** optionNames;
        char** optionDesc;

        int getNumOptions();
        int optionIndex;
        bool active;

        MenuID getMenuID();

        void Free(bool freeStrings = false);
};