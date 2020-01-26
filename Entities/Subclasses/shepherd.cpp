#include "shepherd.h"

Shepherd::Shepherd(int spawnX, int spawnY) {
    id = 1;

    MaxHealth = ShepherdHealth;
    Health = MaxHealth;

    x = spawnX;
    y = spawnY;
}

void Shepherd::SlingFireball(Entity** entities, Particle* particles) {
    HasFire = false;
    animation = 2;      //Fireball Toss animation
    
    AppendEntity(entities, new Fireball(x, y, lastX, lastY, 0));
}
void Shepherd::SwingAttack(Entity** entities, Particle* particles) {
    animation = 2;
    ActivateParticle(particles, 1, x, y);

    int sheepFound = 0;
    int pausedSheep = 0;

    // Tally sheep; find out how many there are and what proportion of them is paused
    for (int w = x-1; w < x+2; w++) {
        for (int z = y-1; z < y+2; z++) {
            Entity* obj = GetEntityAtLocation(entities, w, z);
            if (obj && obj->GetID() == 2) {
                sheepFound++;
                if (obj->Paused)
                    pausedSheep++;
            }
        }
    }

    // Defaultly pause all sheep encountered
    bool NewPause = true;
    if (pausedSheep > sheepFound/2) // ...unless there more paused sheep already than unpaused within the sample 
        NewPause = false;

    // Pause any sheep and attack any wolves within a tile radius of the Shepherd
    for (int w = x-1; w < x+2; w++) {
        for (int z = y-1; z < y+2; z++) {
            Entity* obj = GetEntityAtLocation(entities, w, z);
            if (obj) {
                if (obj->GetID() == 2) {  //If sheep, toggle pause
                    obj->Paused = NewPause;
                    obj->animation = 1;
                } else if (obj->GetID() == 4)   //If wolf, deal damage
                    obj->TakeDamage(1, this);
                else if (obj->GetID() == 6) {
                    Torch* t = dynamic_cast<Torch*>(obj);
                    if (t) {
                        if (!HasFire && !HasFrost && t->FireUsable) {
                            HasFire = obj->HasFire;
                            HasFrost = obj->HasFrost;
                            t->Extinguish();
                        } else if (!t->Extinguishable && (HasFire || HasFrost)) {
                            t->HasFire = HasFire;
                            t->HasFrost = HasFrost;
                        }
                        
                        t->UpdateAnimationData();
                    }
                }
            }
        }
    }
}