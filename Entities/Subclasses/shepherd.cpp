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

    // Pause any sheep and attack any wolves within a tile radius of the Shepherd
    for (int w = x-1; w < x+2; w++) {
        for (int z = y-1; z < y+2; z++) {
            Entity* obj = GetEntityAtLocation(entities, w, z);
            if (obj) {
                if (obj->GetID() == 2) {  //If sheep, toggle pause
                    //obj->Paused = !obj->Paused;
                    //printf("Found sheep at %i, %i and set toggle to %i\n", w, z, obj->Paused ? 1 : 0);
                }
            }
        }
    }
}