#include "Entities/Subclasses/lever.h"

Lever::Lever(int xPos, int yPos) {
    id = EntityID::EE_Lever;

    x = xPos;
    y = yPos;
}

void Lever::ToggleLock(bool lock) {
    Locked = lock;
}

void Lever::Flip() {
    if (!Locked)
        Flipped = !Flipped;
}
bool Lever::IsFlipped() {
    return Flipped;
}
bool Lever::IsLocked() {
    return Locked;
}


void Lever::Tick() {
    if (lastState != Flipped) {
        stateChanged = true;
    } else {
        stateChanged = false;
    }
    lastState = Flipped;
}


void Lever::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    SDL_Rect src;
    src.h = 32;
    src.w = 32;
    src.x = 0; // Default to unflipped texture
    src.y = 0;

    if (Flipped) {  // Flipped texture
        src.x = 32;
    }
    
    SDL_RenderCopyEx(canvas, texture, &src, tile, 0, NULL, SDL_FLIP_NONE);

    if (Locked) { // Layer Lock ontop of lever
        src.y = 32;
        SDL_RenderCopyEx(canvas, texture, &src, tile, 0, NULL, SDL_FLIP_NONE);
    }
}