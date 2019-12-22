#pragma once

#include "Core/config.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/entity_management.h"

#include "Entities/particle.h"

// Shepherd - A character that cares for and commands a flock of sheep. Typically for player.
class Shepherd : public Entity {
    public:
        Shepherd(int spawnX, int spawnY);

    private:

    public:
        int lastX = 0;
        int lastY = 0;

        // Causes Shepherd to sling a fireball; based off last direction they moved
        void SlingFireball(Entity** entities, Particle* particles);


        /* Shepherd does an area-of-effect attack that:
        - Freezes or rallies nearby sheep
        - Hurts any nearby wolves
        */
        void SwingAttack(Entity** entities, Particle* particles);
};