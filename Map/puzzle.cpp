#include "Map/puzzle.h"

void Puzzle_InitializePuzzle(struct Puzzle* puzzle) {
    puzzle->Enabled = false;
    puzzle->Solved = false;
    puzzle->SolvedOnce = false;

    puzzle->entities = (Entity**) calloc(MaxEntitiesPuzzle, sizeof(Entity));
    for (int i = 0; i < MaxEntitiesPuzzle; i++) {
        puzzle->entities[i] = nullptr;
    }

    puzzle->FireType = 1;
    puzzle->PlatesPressed = 0;
    puzzle->LeversFlipped = 0;
}

void Puzzle_FreePuzzle(struct Puzzle* puzzle) {
    free(puzzle->entities);
}

void Puzzle_CheckSolution(struct Puzzle* puzzle) {
    if (!puzzle->Enabled) return;

    int torches = 0;
    int levers = 0;

    int pressurePlatesDown = 0;
    int torchesLit = 0;
    int leversFlipped = 0;

    for (int i = 0; i < MaxEntitiesPuzzle; i++) {
        if (puzzle->entities[i]) {
            Entity* a = puzzle->entities[i];

            if (a->GetID() == 5 && a->OnPressurePlate) {
                pressurePlatesDown++;
            } else if (a->GetID() == 6) {
                torches++;
                
                if (puzzle->FireType == 1 && a->HasFire)
                    torchesLit++;
                else if (puzzle->FireType == 2 && a->HasFrost)
                    torchesLit++;
                else if (puzzle->FireType == 0 && !a->HasFire && !a->HasFrost)
                    torchesLit++;
                else if (puzzle->FireType == -1 && (a->HasFire || a->HasFrost))
                    torchesLit++;
            }
        }
    }

    puzzle->Solved = (torches == torchesLit && pressurePlatesDown == puzzle->PlatesPressed && leversFlipped == puzzle->LeversFlipped);
    if (puzzle->Solved)
        puzzle->SolvedOnce = true;
}