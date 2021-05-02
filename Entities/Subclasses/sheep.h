#pragma once

#include "Entities/entity.h"

#include "Entities/AI/path_manager.h"

// Sheep - A simple sheep that follows the player around
// Can be paused and settled, but are required to move onto any new area
class Sheep : public Entity {
    public:
        Sheep(int xPos, int yPos);

    private:

    public:
        virtual void Unload() override;

        // Called when the entity takes damage; returns true if killed
        // -   If the sheep is armored, it deals 3 damage to the attacker and loses its armor
        virtual bool TakeDamage(int dmgAmount, Entity* attacker) override;


        virtual void Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) override;

        // Armored - If a sheep is armored, they have an additional point of health
        // Any wolves that attack an armored sheep are destroyed at the cost of the armor
        bool Armored = false;

        // Holds pathdata
        Path* currentPath = nullptr;
};