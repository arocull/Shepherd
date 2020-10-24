#pragma once

#include "Core/config.h"
#include "Core/enums.h"
#include "Core/mathutil.h"

#include "Entities/entity.h"
#include "Entities/particle.h"

// Spirit - The power of the Holy Spirit resonates within you
// - Floats around interacting with the world around it, and emitting particles
class Spirit : public Entity {
    public:
        Spirit(int xPos, int yPos);

        int goalX;
        int goalY;
    private:
        float emissionAngle = 0.0f;

    public:
        bool hasReachedGoal();
        void Emit(Particle* particles);
};