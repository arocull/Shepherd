#include "Entities/particle.h"


struct Particle* ActivateParticle(Particle** particleArray, int id, int x, int y) {
    Particle* particle = nullptr;
    for (int i = 0; i < MaxParticles; i++) {
        if (particleArray[i] && particleArray[i]->active)
            particle = particleArray[i];
    }
    if (particle) {
        particle->lifetime = 0;
        particle->x = x;
        particle->y = y;
        particle->id = id;
        particle->active = true;
    }

    return particle;
}
void DeactivateParticle(Particle* particle) {
    particle->active = false;
    particle->veloX = 0;
    particle->veloY = 0;
    particle->rate = 1;
    particle->loop = false;
}


void TickParticles(Particle** particleArray, float deltaTime) {
    for (int i = 0; i < MaxParticles; i++) {
        if (particleArray[i] && particleArray[i]->active) {
            particleArray[i]->lifetime+=deltaTime;

            particleArray[i]->x += particleArray[i]->veloX*deltaTime;
            particleArray[i]->y += particleArray[i]->veloY*deltaTime;
        
            if (particleArray[i]->lifetime > particleArray[i]->maxLifetime) {
                if (particleArray[i]->loop)
                    particleArray[i]->lifetime -= particleArray[i]->maxLifetime;
                else
                    DeactivateParticle(particleArray[i]);
            }
        }
    }
}


void StopParticles(Particle** particleArray) {
    for (int i = 0; i < MaxParticles; i++) {
        DeactivateParticle(particleArray[i]);
    }
}