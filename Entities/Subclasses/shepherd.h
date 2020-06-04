#pragma once

#include "Core/config.h"
#include "Core/enums.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/lever.h"
#include "Entities/entity_management.h"

#include "Entities/particle.h"

// Shepherd - A character that cares for and commands a flock of sheep. Typically for player.
class Shepherd : public Entity {
    public:
        Shepherd(int spawnX, int spawnY);

    private:

    public:
        int faceX = 1;
        int faceY = 0;

        int ticksIdled = 0;

        virtual void Tick() override;

        // Causes Shepherd to sling a fireball; based off last direction they moved
        void SlingFireball(Entity** entities, Particle* particles);


        /* Shepherd does an area-of-effect attack that:
        - Freezes or rallies nearby sheep
        - Hurts any nearby wolves
        */
        void SwingAttack(Entity** entities, Particle* particles);
};