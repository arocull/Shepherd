#pragma once

// Input Action - Struct to hold data for working with controller inputs
struct InputAction {
    bool close; // If true, game will forcibly close
    bool pause; // If true, game will attempt to pause
    bool fullscreen; // If true, game will toggle fullscreen
    int index; // Generic index of the selected option, -1 for nothing
};