#include "Entities/Subclasses/spirit.h"

Spirit::Spirit(int xPos, int yPos) {
    id = EntityID::EE_Spirit;
    x = xPos;
    y = yPos;

    MaxHealth = 100;
    Health = 100;
    Solid = false;

    archivable = false;
}



bool Spirit::hasReachedGoal() {
    return (x == goalX && y == goalY);
}

void Spirit::Emit(Particle* particles) {
    emissionAngle += PI / 4;
    if (emissionAngle >= 2 * PI) emissionAngle = 0;

    Particle* a = ActivateParticle(particles, ParticleID::EP_Spirit, x, y);
    if (a) {
        a->maxLifetime = 0.25f;
        a->veloX = 3 * cos(emissionAngle);
        a->veloY = 3 * sin(emissionAngle);
        printf("Angle %f,\t%f %f\n", emissionAngle, a->veloX, a->veloY);
    }
    a = ActivateParticle(particles, ParticleID::EP_Spirit, x, y);
    if (a) {
        a->maxLifetime = 0.4f;
    }
}