#include "fireball.h"

Fireball::Fireball(int spawnX, int spawnY, int dirX, int dirY, int type) {
    id = 3;
    x = spawnX;
    y = spawnY;
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
            if (hit && !hit->HasFire) {

                // If it hits a torch, attempt to ignite it
                if (hit->GetID() == 6) {
                    Torch* torch = dynamic_cast<Torch*>(hit);
                    if (torch && torch->Extinguishable) {
                        torch->HasFire = HasFire;
                        torch->HasFrost = HasFrost; 
                    }
                // If it hits a crate, incinerate it
                } else if (hit->GetID() == 5) {
                    Crate* crate = dynamic_cast<Crate*>(hit);
                    if (crate && crate->canIncinerate) crate->Incinerate(particles, this);
                } else {
                    hit->HasFire = HasFire;
                    hit->HasFrost = HasFrost;  

                    if (!((hit->GetID() == 1 || hit->GetID() == 2) && !enemy))
                        hit->TakeDamage(1, this);
                }
            }
        }
    }

    return RemoveEntity(entities, this);
}