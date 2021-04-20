#include "wolf.h"

Wolf::Wolf(int xPos, int yPos) {
    id = EntityID::EE_Wolf;

    MaxHealth = 2;
    Health = MaxHealth;

    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;

    target = nullptr;
}

// Called upon unloading--overriden from base entity class
// Resets the wolf's stun and forcefully ends their hunt
void Wolf::Unload() {
    EndHunt();
    stun = 0;

    Path_FreePath(currentPath);
    currentPath = nullptr;
}



// Is the wolf stunned (in a state such as howling or has recently been hit)?
bool Wolf::InStun() {
    return (stun > 0);
}
// Tick the wolf's stun down by one.
void Wolf::TickStun() {
    stun--;
}

// Howl and begin pursuit of new target.
void Wolf::InitiateHunt(Entity* newTarget) {
    target = newTarget;
    animation = AnimationID::ANIM_Attack;
    stun = 5;
}
// Forcefully end the current hunt, clearing target and having the wolf pause for a second.
// Use when unloading wolves to prevent crashing from attempting to pursue non-existant entities
void Wolf::EndHunt() {
    target = nullptr;
    animation = AnimationID::ANIM_Idle;
    stun = 1;
}
// Is the wolf currently pursuing an existing entity that isn't dead?
bool Wolf::IsHunting() {
    if (target && target->GetHealth() > 0)
        return true;
    else
        return false;
}

// Returns the target the wolf is currently pursuing
Entity* Wolf::GetTarget() {
    return target;
}