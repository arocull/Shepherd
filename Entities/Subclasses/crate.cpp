#include "Entities/Subclasses/crate.h"

Crate::Crate(int xPos, int yPos) {
    id = EntityID::EE_Crate;

    x = xPos;
    y = yPos;
}

void Crate::Incinerate(Particle* particles, Entity* attacker) {
    if (TakeDamage(1, attacker)) {
        Particle* burnEffect = ActivateParticle(particles, ParticleID::EP_Incinerate, x, y);
        burnEffect->maxLifetime = 1.0f;
    }
}