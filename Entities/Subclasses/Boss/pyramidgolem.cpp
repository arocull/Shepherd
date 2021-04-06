#include "pyramidgolem.h"

PyramidGolem::PyramidGolem(int xPos, int yPos) {
    id = EntityID::EE_PyramidGolem;

    MaxHealth = 3;
    Health = MaxHealth;

    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;
    startY = yPos;

    fireballTimer = -TickRate; // Give player a little extra time at beginning to set up
}

// Called upon unloading--overriden from base entity class
// Resets the PyramidGolem's stun
void PyramidGolem::Unload() {
    stun = 0;
    target = nullptr;
}
// Called every tick
void PyramidGolem::Tick() {
    goalMoveX = 0;
    goalMoveY = 0;

    if (InStun()) { // Do nothing if stunned
        TickStun();
        return;
    }

    goalMoveY = sgn(y - startY); // Move back to goal Y position if moved out of way
    otherTick = !otherTick; // Move horizontally every other tick
    fireballTimer++;
    if (!target || otherTick) return;
    int goalPosX = target->x;
    goalPosX = min(max(3, goalPosX), 37); // Don't get too close to map edges

    goalMoveX = sgn(goalPosX - x); // Move toward target
    if (goalMoveY != 0) goalMoveX = 0; // Don't move horizontally if moving vertically

    if (fireballTimer > 14 && (target->x == this->x || target->lastX == this->x)) {
        tossFireball = true;
    }
}


// Goal X move direction
int PyramidGolem::GetGoalX() {
    return goalMoveX;
}
// Goal Y move direction
int PyramidGolem::GetGoalY() {
    return goalMoveY;
}
// Returns true if the boss should toss a fireball, and sets itself to false afterward
bool PyramidGolem::DoFireballToss() {
    bool tossValue = tossFireball;
    if (tossFireball) {
        tossFireball = false;
        fireballTimer = 0;
    }
    return tossValue;
}


bool PyramidGolem::TakeDamage(int dmgAmount, Entity* attacker) {
    printf("Hit\n");
    if (attacker && attacker->GetID() == EntityID::EE_Fireball) {
        bool died = Entity::TakeDamage(dmgAmount, attacker);

        stun += 15;
        y--; // Get knocked back 1
        if (y < 1) y = 1; // Dont get knocked into wall

        printf("Hit and stunned\n");

        return died;
    }
    return false;
}


// Is the PyramidGolem stunned (in a state such as howling or has recently been hit)?
bool PyramidGolem::InStun() {
    return (stun > 0);
}
// Tick the PyramidGolem's stun down by one.
void PyramidGolem::TickStun() {
    stun--;
}


void PyramidGolem::SetTarget(Entity* newTarget) {
    target = newTarget;
}
Entity* PyramidGolem::GetTarget() {
    return target;
}