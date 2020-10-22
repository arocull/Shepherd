#include "Entities/Subclasses/spirit.h"

Spirit::Spirit(int xPos, int yPos) {
    id = EntityID::EE_Spirit;
    x = xPos;
    y = yPos;

    MaxHealth = 100;
    Health = 100;
    Solid = false;
}



bool Spirit::hasReachedGoal() {
    return (x == goalX && y == goalY);
}

void Spirit::Emit(Particle* particles) {
    emissionAngle += PI / 4;
    if (emissionAngle > 2 * PI) emissionAngle = 0;

    Particle* a = ActivateParticle(particles, ParticleID::EP_Spirit, x, y);
    a->maxLifetime = 0.5f;
    a->veloX = cos(emissionAngle);
    a->veloY = sin(emissionAngle);
}