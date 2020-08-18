#pragma once

#include <cstdlib>

// Menu - A UI class that holds various data for displaying such as string lists
class Menu {
    public:
        Menu(int numItems);
    
    private:
        int numOptions;

    public:
        char** optionNames;
        char** optionDesc;

        int getNumOptions();
        int optionIndex;
        bool active;

        void Free(bool freeStrings = false);
};