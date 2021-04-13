#include "pyramidgolem.h"

PyramidGolem::PyramidGolem(int xPos, int yPos) {
    id = EntityID::EE_PyramidGolem;

    MaxHealth = 4;
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
        HasFire = false;
    }
    return tossValue;
}
void PyramidGolem::UpdateFireVisual() {
    HasFire = fireballTimer >= 9;
}


bool PyramidGolem::TakeDamage(int dmgAmount, Entity* attacker) {
    printf("Hit\n");
    // Fireball attack knocks golem down, but does not damage it
    if (attacker && attacker->GetID() == EntityID::EE_Fireball) {
        UpdateFireVisual();

        // Stun states (for briding + smashing)
        enteredStun = true;
        exitedStun = false;

        stun += 45;
        lastY = y;
        y--; // Get knocked back 1
        if (y < 1) y = 1; // Dont get knocked into wall

        return false;
    }
    // Player swing attacks are what actually damage the golem
    if (attacker && attacker->GetID() == EntityID::EE_Shepherd) {
        bool died = Entity::TakeDamage(dmgAmount, attacker);

        stun = 1; // End stun and knock players back

        lastY = y;
        y--; // Get knocked back 1
        if (y < 1) y = 1; // Dont get knocked into wall

        lastX = x;
        // Get knocked to side
        x += sgn(x - attacker->x);
        x = min(max(x, 1), MapWidth - 2); // Prevent clipping, again

        attacker->HasFire = false; // Ensure Shepherd does not gain fire

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

bool PyramidGolem::ShouldBridge() {
    return enteredStun;
}
bool PyramidGolem::ShouldSmash() {
    return (exitedStun == false && stun <= 0);
}
void PyramidGolem::BuildBridge(Map* map) {
    enteredStun = false;

    map->FillRectangle(x - 1, 4, x + 2, 10, TileID::ET_Empty_Puzzle_Piece);
}
void PyramidGolem::Smash(Map* map, Entity** entities) {
    exitedStun = true;
    map->FillRectangle(1, 4, MapWidth - 2, 10, TileID::ET_Empty_Tile);

    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i] != this) {
            ThrowEntity(entities[i], entities, map);
        }
    }

    // Give player a tiny bit of time to recooperate and gather sheep
    stun += 3;
    fireballTimer = -5;
}
void PyramidGolem::ThrowEntity(Entity* hit, Entity** entities, Map* map) {
    if (hit->GetID() == EntityID::EE_Shepherd) {
        hit->Pause(); // Stun shepherd
    } else {
        hit->Paused = false; // Force sheep to get up (frightened, adds to chaos)
    }

    int hitX = hit->x;
    int hitY = hit->y;
    
    // Randomize vertical location
    hit->y = RandomI(10, MapHeight-2);

    // Shift entity to prevent entities from getting stacked ontop of each other
    Movement_ShiftEntity(map, entities, hit, sgn(x - hitX) * RandomI(1, 3), -1, true);
    
    hit->lastX = hitX;
    hit->lastY = hitY;
}


void PyramidGolem::SetTarget(Entity* newTarget) {
    target = newTarget;
}
Entity* PyramidGolem::GetTarget() {
    return target;
}