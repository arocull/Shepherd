#include "Entities/Subclasses/lever.h"

Lever::Lever(int xPos, int yPos) {
    id = EntityID::EE_Lever;

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
bool Lever::IsLocked() {
    return Locked;
}


void Lever::UpdateAnimationData() {
    if (Locked)
        animationMetadata = 1;
    else
        animationMetadata = 0;
    
    if (Flipped)
        animation = AnimationID::ANIM_Walk;
    else
        animation = AnimationID::ANIM_Idle;
}

void Lever::Tick() {
    UpdateAnimationData();
    if (lastState != Flipped) {
        stateChanged = true;
    } else {
        stateChanged = false;
    }
    lastState = Flipped;
}