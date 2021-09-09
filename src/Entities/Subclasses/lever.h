#pragma once

#include "Entities/entity.h"

// Lever - A lever that can be flipped and toggled with staff swings and fireballs.
class Lever : public Entity {
    public:
        Lever(int posX, int posY);

    private:
        bool Locked = false;
        bool lastState = false;

    public:
        void ToggleLock(bool lock);

        bool stateChanged = false;
        
        void Flip();
        bool IsFlipped();
        bool IsLocked();

        virtual void Tick() override;
        virtual void Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) override;

        // Generates an Ascii string that contains data on this entity. See Entity::Ascii
        virtual std::string* Ascii() override;
        // Loads data from a given ascii string. See Entity::LoadAscii
        virtual void LoadAscii(char* str, int* index) override;
};