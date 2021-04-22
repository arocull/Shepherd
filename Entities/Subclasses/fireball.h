#pragma once

#include "Core/config.h"
#include "Core/enums.h"

#include "Entities/entity.h"

#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/crate.h"
#include "Entities/Subclasses/lever.h"

#include "Entities/particle.h"

#include "Entities/entity_management.h"

// Fireball - Mobile projectile that can be slung by Shepherds and enemies
// - Fireballs have destructive properties and can send fire types over a distance, while also interacting with puzzle mechanics
class Fireball : public Entity {
    public:
        Fireball(int spawnX, int spawnY, int dirX, int dirY, int type);

        int speedX;
        int speedY;
        // Set to true if the fireball was cast by an enemy and should damage the Shepherd
        bool enemy = false;

        void Burst(Entity** entities, Particle* particles);

        virtual void Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) override;
    
    private:
        void BurstTorch(Torch* torch);
        void BurstCrate(Crate* crate, Particle* particles);
        void BurstLever(Lever* lever);
        void BurstFriendly(Entity* hit);
        void BurstEnemy(Entity* hit);
};