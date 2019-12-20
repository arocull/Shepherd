#pragma once

#include "config.h"

#include "entity.h"
#include "entity_management.h"

#include "particle.h"

// Shepherd - A character that cares for and commands a flock of sheep. Typically for player.
class Shepherd : public Entity {
    public:
        Shepherd(int spawnX, int spawnY);

    private:

    public:
        int lastX = 0;
        int lastY = 0;

        void SwingAttack(Entity** entities, Particle* particles);
};