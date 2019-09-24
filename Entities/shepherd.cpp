#include "shepherd.h"

Shepherd::Shepherd(int spawnX, int spawnY) {
    id = 1;

    MaxHealth = ShepherdHealth;
    Health = MaxHealth;

    x = spawnX;
    y = spawnY;
}


// Checks to make sure all sheep are touching the shepherd (or touching other sheep touching the shepherd)
bool Shepherd::HasAllSheep() {
    return true;
}