#include "Entities/Subclasses/torch.h"

Torch::Torch(int xPos, int yPos) {
    id = EntityID::EE_Torch;

    x = xPos;
    y = yPos;

    Solid = true;
    HasFire = true;
    UpdateAnimationData();
}
void Torch::Tick() {
    UpdateAnimationData();
}


void Torch::UpdateAnimationData() {
    if (HasFire)
        animationMetadata = 1;
    else if (HasFrost)
        animationMetadata = 2;
    else
        animationMetadata = 0;
    
    if (glow)
        animation = AnimationID::ANIM_Walk;
    else
        animation = AnimationID::ANIM_Idle;
}

void Torch::Extinguish(bool Override) {
    if (Extinguishable || Override) {
        HasFire = false;
        HasFrost = false;
        ResetAnimationTimers();
    }
}

void Torch::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    animationTimerTicks++;

    SDL_Rect src;
    src.h = 32;
    src.w = 32;
    src.x = 0;
    src.y = 0;

    if (animation == AnimationID::ANIM_Walk) {  // Glowing Base Texture
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