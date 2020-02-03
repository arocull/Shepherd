#include "Entities/Subclasses/lever.h"

Lever::Lever(int xPos, int yPos) {
    id = 7;

    x = xPos;
    y = yPos;
}

void Lever::ToggleLock(bool lock) {
    Locked = lock;
}

void Lever::Flip() {
    if (!Locked)
        Flipped = !Flipped;
}
bool Lever::IsFlipped() {
    return Flipped;
}


void Lever::UpdateAnimationData() {
    if (Locked)
        animationMetadata = 1;
    else
        animationMetadata = 0;
    
    if (Flipped)
        animation = 1;
    else
        animation = 0;
}

void Lever::Tick() {
    UpdateAnimationData();
}