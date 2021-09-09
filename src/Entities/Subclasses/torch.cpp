#include "Entities/Subclasses/torch.h"

Torch::Torch(int xPos, int yPos) {
    id = EntityID::EE_Torch;

    x = xPos;
    y = yPos;

    Solid = true;
    HasFire = true;
}

void Torch::Extinguish(bool Override) {
    if (Extinguishable || Override) {
        HasFire = false;
        HasFrost = false;
        ResetAnimationTimers();
    }
}

void Torch::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    SDL_Rect src;
    src.h = 32;
    src.w = 32;
    src.x = 0;
    src.y = 0;

    if (glow) {  // Glowing Base Texture
        src.x = 32;
    }
    
    // Draw base texture
    SDL_RenderCopyEx(canvas, texture, &src, tile, 0, NULL, GetFlipStyle());

    // Overlay fire
    if (HasFire || HasFrost) {
        src.x = (animationTimerTicks % 2) * 32;
        if (HasFrost) src.y = 64;
        else src.y = 32;

        SDL_RenderCopyEx(canvas, texture, &src, tile, 0, NULL, GetFlipStyle());
    }
}


std::string* Torch::Ascii() {
    std::string* text = Entity::Ascii();
    strutil::appendChar(text, ',');
    strutil::poolIntegers(text, 3, glow, Extinguishable, FireUsable);
    return text;
}
void Torch::LoadAscii(char* str, int* index) {
    Entity::LoadAscii(str, index);
    glow = strutil::parseBool(str, index);
    Extinguishable = strutil::parseBool(str, index);
    FireUsable = strutil::parseBool(str, index);
}