#pragma once

#include "Entities/entity.h"

// Lever - A lever that can be flipped and toggled with staff swings.
class Lever : public Entity {
    public:
        Lever(int posX, int posY);

    private:
        bool Flipped = false;
        bool Locked = false;

        void UpdateAnimationData();

    public:
        void ToggleLock(bool lock);
        
        void Flip();
        bool IsFlipped();

        virtual void Tick() override;
};