#pragma once

#include "Entities/entity.h"


// Crate - A box that can be pushed around to solve various puzzles
class Crate : public Entity {
    public:
        Crate(int xPos, int yPos);

        // Crate type--can be used to create sliding puzzles where different crates have different destinations
        int type = 0;
};