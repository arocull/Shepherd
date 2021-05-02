#include "Entities/Subclasses/sheep.h"

Sheep::Sheep(int xPos, int yPos) {
    id = EntityID::EE_Sheep;
    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;
    MaxHealth = 2;
    Health = 2;
}

void Sheep::Unload() {
    Path_FreePath(currentPath);
    currentPath = nullptr;
}


bool Sheep::TakeDamage(int dmgAmount, Entity* attacker) {
    if (Armored) {
        attacker->TakeDamage(3, this);
        Armored = false;
    }

    Health-=dmgAmount;
    ResetAnimationTimers();

    return Health <= 0;
}


void Sheep::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    SDL_Rect src;
    src.w = 32;
    src.h = 32;
    src.x = 0;
    src.y = 0;

    double angle = 0.0;
    int step = (animationTimerTicks / 2) % 4;

    switch (animation) {
        case AnimationID::ANIM_Paused:
            src.x = 64;
            break;
        case AnimationID::ANIM_Walk: // Set angle based off of range of 1 to 3
            switch (step) {
                case 0:
                    angle = 3.0;
                    break;
                case 2:
                    angle = -3.0;
                    break;
                default:
                    angle = 0;
                    break;
            }
        case AnimationID::ANIM_Idle:
        default:
            src.x = (animationTimerTicks/2 % 2) * 32;
            break;
    }

    SDL_RenderCopyEx(canvas, texture, &src, tile, angle, NULL, GetFlipStyle());

    // Overlay blood
    if (Health < MaxHealth) {
        src.y = 32;
        SDL_RenderCopyEx(canvas, texture, &src, tile, angle, NULL, GetFlipStyle());
    }
    // Overlay armor (not used as of currently)
}