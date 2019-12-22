#include "Entities/particle.h"


struct Particle* ActivateParticle(Particle* particleArray, int id, int x, int y) {
    Particle* particle = nullptr;
    for (int i = 0; i < MaxParticles; i++) {
        if (!particleArray[i].active) {
            particle = &particleArray[i];
            break;
        }
    }

    if (particle) {     // Reset particle stats (not done upon initialization so must be done here)
        particle->lifetime = 0.0f;
        particle->maxLifetime = 0.25f;
        particle->x = x;
        particle->y = y;
        particle->veloX = 0.0f;
        particle->veloY = 0.0f;
        particle->id = id;
        particle->loop = false;
        particle->active = true;
    }

    return particle;
}
void DeactivateParticle(Particle* particle) {
    particle->active = false;
}


void TickParticles(Particle* particleArray, float deltaTime) {
    for (int i = 0; i < MaxParticles; i++) {
        if (particleArray[i].active) {
            particleArray[i].lifetime+=deltaTime;

            particleArray[i].x += particleArray[i].veloX*deltaTime;
            particleArray[i].y += particleArray[i].veloY*deltaTime;
            if (particleArray[i].lifetime > particleArray[i].maxLifetime) {
                if (particleArray[i].loop)
                    particleArray[i].lifetime -= particleArray[i].maxLifetime;
                else
                    DeactivateParticle(&particleArray[i]);
            }
        }
    }
}


void StopParticles(Particle* particleArray) {
    for (int i = 0; i < MaxParticles; i++)
        DeactivateParticle(&particleArray[i]);
}