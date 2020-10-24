#pragma once

#include "Core/config.h"

#include <cstdio>

/*
Particle data-structure

Holds information about particles and what they're doing
*/
struct Particle {
    float x;
    float y;
    int id;
    float lifetime;
    float maxLifetime;
    float veloX;
    float veloY;
    bool loop;  // Do you want to loop animations on this particle?
    bool active;
};

// Activates and returns the first available particle with the given properties
// Note, may return a nullptr if no particles are available--be sure to check this case
struct Particle* ActivateParticle(Particle* particleArray, int id, int x, int y);
// Deactivates the selected particle
void DeactivateParticle(Particle* particle);

// Deactivates any timed-out particles or resets lifespan on looping ones
void TickParticles(Particle* particleArray, float DeltaTime);
// Forcibly deactivates all given particles
void StopParticles(Particle* particleArray);