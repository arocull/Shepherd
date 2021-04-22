#pragma once

#include "Entities/entity.h"


// Torch - A torch that can hold different types of fire that the player can access and manipulate
class Torch : public Entity {
    public:
        Torch(int xPos, int yPos);
        virtual void Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) override;

        // Do we want the torch to have a glowing effect (gives 'activated' sort of feel)?
        bool glow = false;

        // Can the fire be manipulated or extinguished by the player? 
        bool Extinguishable = true;
        // Is the player able to retrieve the fire from this torch?
        bool FireUsable = true;
        
        // Extinguishes the torch (both fire and frost)
        // - Only works if the torch can be extinguished (unless overriden)
        void Extinguish(bool Override = false);
};