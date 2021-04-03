#include "pyramidgolem.h"

PyramidGolem::PyramidGolem(int xPos, int yPos) {
    id = EntityID::EE_PyramidGolem;

    MaxHealth = 2;
    Health = MaxHealth;

    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;
}

// Called upon unloading--overriden from base entity class
// Resets the PyramidGolem's stun
void PyramidGolem::Unload() {
    stun = 0;

    Path_FreePath(currentPath);
    currentPath = nullptr;
}



// Is the PyramidGolem stunned (in a state such as howling or has recently been hit)?
bool PyramidGolem::InStun() {
    return (stun > 0);
}
// Tick the PyramidGolem's stun down by one.
void PyramidGolem::TickStun() {
    stun--;
}