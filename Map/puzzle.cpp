#include "Map/puzzle.h"

void Puzzle_InitializePuzzle(struct Puzzle* puzzle) {
    puzzle->Enabled = false;
    puzzle->Solved = false;
    puzzle->SolvedOnce = false;

    puzzle->entities = (Entity**) calloc(MaxEntitiesPuzzle, sizeof(Entity*));
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
    int wolves = 0;

    int pressurePlatesDown = 0;
    int torchesLit = 0;
    int leversFlipped = 0;

    for (int i = 0; i < MaxEntitiesPuzzle; i++) {
        if (puzzle->entities[i]) {
            Entity* a = puzzle->entities[i];

            if (a->GetID() == EntityID::EE_Wolf && a->GetHealth() > 0) {
                wolves++;
            } else if (a->GetID() == EntityID::EE_Crate && a->OnPressurePlate) {
                pressurePlatesDown++;
            } else if (a->GetID() == EntityID::EE_Torch) {
                torches++;
                
                if (puzzle->FireType == 1 && a->HasFire)
                    torchesLit++;
                else if (puzzle->FireType == 2 && a->HasFrost)
                    torchesLit++;
                else if (puzzle->FireType == 0 && !a->HasFire && !a->HasFrost)
                    torchesLit++;
                else if (puzzle->FireType == -1 && (a->HasFire || a->HasFrost))
                    torchesLit++;
            } else if (a->GetID() == EntityID::EE_Lever) {
                Lever* l = dynamic_cast<Lever*>(a);
                if (l && l->IsFlipped())
                        leversFlipped++;
            }
        }
    }

    puzzle->Solved = (
        torches == torchesLit &&
        pressurePlatesDown == puzzle->PlatesPressed &&
        leversFlipped == puzzle->LeversFlipped &&
        wolves <= 0
    );
    if (puzzle->Solved)
        puzzle->SolvedOnce = true;
}