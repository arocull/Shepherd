#include "Entities/Subclasses/sheep.h"

Sheep::Sheep(int xPos, int yPos) {
    id = 2;
    x = xPos;
    y = yPos;
    MaxHealth = 2;
    Health = 2;

    animationMetadata = Health;
}

void Sheep::Unload() {
    Path_FreePath(currentPath);
}

bool Sheep::TakeDamage(int dmgAmount, Entity* attacker) {
    if (Armored) {
        attacker->TakeDamage(3, this);
        Armored = false;
    }

    Health-=dmgAmount;

    animationMetadata = Health;

    return Health <= 0;
}