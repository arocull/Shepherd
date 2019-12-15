#pragma once

#include "Core/config.h"

/*
Particle data-structure

Holds information about particles and what they're doing
*/
struct Particle {
    float x;
    float y;
    int id = -1;
    float lifetime = 0;
    float maxLifetime = 1;
    float rate = 1;     // Tick rate of particle
    float veloX;
    float veloY;
    bool loop = false;  // Do you want to loop animations on this particle?
    bool active = false;
};

// Activates and returns the first available particle with the given properties
struct Particle* ActivateParticle(Particle** particleArray, int id, int x, int y);
// Deactivates the selected particle
void DeactivateParticle(Particle* particle);

// Deactivates any timed-out particles or resets lifespan on looping ones
void TickParticles(Particle** particleArray, float DeltaTime);
// Forcibly deactivates all given particles
void StopParticles(Particle** particleArray);