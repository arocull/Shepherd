#include "fireball.h"

Fireball::Fireball(int spawnX, int spawnY, int dirX, int dirY, int type) {
    id = EntityID::EE_Fireball;
    x = spawnX;
    y = spawnY;
    lastX = spawnX;
    lastY = spawnY;
    speedX = dirX*FireballSpeed;
    speedY = dirY*FireballSpeed;
    
    if (type > 1) {
        HasFrost = true;
        animation = 1;
    } else {
        HasFire = true;
        animation = 0;
    }
}

// Causes the fireball to burst, setting nearby entities on fire as well as deleting the fireball itself
void Fireball::Burst(Entity** entities, Particle* particles) {
    Particle* explosionEffect = ActivateParticle(particles, 5, x, y);

    for (int iX = -1; iX < 2; iX++) {
        for (int iY = -1; iY < 2; iY++) {

            Entity* hit = GetEntityAtLocation(entities, x + iX, y + iY);
            if (hit) {
                switch (hit->GetID()) {
                    case EntityID::EE_Torch:
                        BurstTorch(dynamic_cast<Torch*>(hit));
                        break;
                    case EntityID::EE_Crate:
                        BurstCrate(dynamic_cast<Crate*>(hit), particles);
                        break;
                    case EntityID::EE_Lever:
                        BurstLever(dynamic_cast<Lever*>(hit));
                        break;
                    case EntityID::EE_Shepherd:
                    case EntityID::EE_Sheep:
                        BurstFriendly(hit);
                        break;
                    default:
                        BurstEnemy(hit);
                        break;
                }
            }
        }
    }

    return RemoveEntity(entities, this);
}

void Fireball::BurstTorch(Torch* torch) {
    if (torch && torch->Extinguishable) {
        torch->HasFire = HasFire;
        torch->HasFrost = HasFrost;
    }
};
void Fireball::BurstCrate(Crate* crate, Particle* particles) {
    if (crate && crate->canIncinerate) {
        crate->Incinerate(particles, this);
    }
};
void Fireball::BurstLever(Lever* lever) {
    if (lever) {
        lever->Flip();
    }
};
void Fireball::BurstFriendly(Entity* hit) {
    hit->Pause(); // Stun shepherd and sheep

    if (enemy) { // If the fireball was slung by an enemy, damage them
        hit->TakeDamage(1, this);
    }
};
void Fireball::BurstEnemy(Entity* hit) {
    hit->HasFire = HasFire;
    hit->HasFrost = HasFrost;
    hit->TakeDamage(1, this); // if (!enemy)
};