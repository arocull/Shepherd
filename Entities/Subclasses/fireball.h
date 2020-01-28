#pragma once

#include "Core/config.h"

#include "Entities/entity.h"

#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/crate.h"

#include "Entities/particle.h"

#include "Entities/entity_management.h"


class Fireball : public Entity {
    public:
        Fireball(int spawnX, int spawnY, int dirX, int dirY, int type);

        int speedX;
        int speedY;

        void Burst(Entity** entities, Particle* particles);
};