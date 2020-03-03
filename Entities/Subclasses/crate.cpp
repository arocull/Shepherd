#include "Entities/Subclasses/crate.h"

Crate::Crate(int xPos, int yPos) {
    id = 5;

    x = xPos;
    y = yPos;
}

void Crate::Incinerate(Particle* particles, Entity* attacker) {
    if (TakeDamage(1, attacker)) {
        Particle* burnEffect = ActivateParticle(particles, 6, x, y);
        burnEffect->maxLifetime = 1.0f;
    }
}