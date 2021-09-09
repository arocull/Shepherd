#pragma once

#include "Core/config.h"
#include "Core/enums.h"

#include "Entities/entity.h"
#include "Entities/entity_management.h"

#include "Entities/Subclasses/lever.h"

// Puzzle - A struct that holds puzzle information and automatically handles solutions
struct Puzzle {
    bool Enabled;       // Is this puzzle allowed to be used and solved?
    bool Solved;        // Has this puzzle been solved?
    bool SolvedOnce;    // Has this puzzle been solved ever?

    Entity** entities;  // All entities inside the puzzle that should be checked for solve conditions

    // If there are torches involved in the puzzle, what type of fire do they need to be considered "solved?"
    // - 0 for none, 1 for fire, 2 for frost, -1 for not picky
    // Note: Only counted if torches are present within puzzle data
    int FireType;

    // Exact number of pressure plates in the map that need to be weighed down by entities within this puzzle before it is considered solved
    int PlatesPressed;

    // Exact number of levers that should be flipped on for this puzzle to be considered solved
    // Note: only counted if levers are present within puzzle data
    int LeversFlipped;
};

// Initializes the data of a puzzle structure, filling out defaults and allocating array memory
void Puzzle_InitializePuzzle(struct Puzzle* puzzle);
// Frees the data within a puzzle structure as well as the puzzle itself
void Puzzle_FreePuzzle(struct Puzzle* puzzle);
// Checks puzzle conditions for a solution
void Puzzle_CheckSolution(struct Puzzle* puzzle);