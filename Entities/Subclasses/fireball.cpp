#include "fireball.h"

Fireball::Fireball(int spawnX, int spawnY, int dirX, int dirY, int type) {
    id = EntityID::EE_Fireball;
    x = spawnX;
    y = spawnY;
    lastX = spawnX;
    lastY = spawnY;
    speedX = dirX*FireballSpeed;
    speedY = dirY*FireballSpeed;
    
    if (type > 1) {
        HasFrost = true;
        animation = AnimationID::ANIM_Walk;
    } else {
        HasFire = true;
        animation = AnimationID::ANIM_Idle;
    }
}

// Causes the fireball to burst, setting nearby entities on fire as well as deleting the fireball itself
void Fireball::Burst(Entity** entities, Particle* particles) {
    Particle* explosionEffect = ActivateParticle(particles, 5, x, y);

    for (int iX = -1; iX < 2; iX++) {
        for (int iY = -1; iY < 2; iY++) {

            Entity* hit = GetEntityAtLocation(entities, x + iX, y + iY);
            if (hit) {
                switch (hit->GetID()) {
                    case EntityID::EE_Torch:
                        BurstTorch(dynamic_cast<Torch*>(hit));
                        break;
                    case EntityID::EE_Crate:
                        BurstCrate(dynamic_cast<Crate*>(hit), particles);
                        break;
                    case EntityID::EE_Lever:
                        BurstLever(dynamic_cast<Lever*>(hit));
                        break;
                    case EntityID::EE_Shepherd:
                    case EntityID::EE_Sheep:
                        BurstFriendly(hit);
                        break;
                    default:
                        BurstEnemy(hit);
                        break;
                }
            }
        }
    }

    return RemoveEntity(entities, this);
}

void Fireball::BurstTorch(Torch* torch) {
    if (torch && torch->Extinguishable) {
        torch->HasFire = HasFire;
        torch->HasFrost = HasFrost;
    }
};
void Fireball::BurstCrate(Crate* crate, Particle* particles) {
    if (crate && crate->canIncinerate) {
        crate->Incinerate(particles, this);
    }
};
void Fireball::BurstLever(Lever* lever) {
    if (lever) {
        lever->Flip();
    }
};
void Fireball::BurstFriendly(Entity* hit) {
    hit->Pause(); // Stun shepherd and sheep

    if (enemy) { // If the fireball was slung by an enemy, damage them
        hit->TakeDamage(1, this);
    }
};
void Fireball::BurstEnemy(Entity* hit) {
    hit->HasFire = HasFire;
    hit->HasFrost = HasFrost;
    hit->TakeDamage(1, this); // if (!enemy)
};


void Fireball::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    animationTimerTime += delta;

    int sha = (int) 20 * (sin(animationTimerTime * 20) + 1);
    SDL_SetRenderDrawColor(canvas, 210 + sha, 100 + sha, 0, 0);

    tile->x += tile->w*.25;
    tile->y += tile->h*.25;
    tile->w *= .5;
    tile->h *= .5;

    SDL_RenderFillRect(canvas, tile);
}


std::string* Fireball::Ascii() {
    std::string* text = Entity::Ascii();
    strutil::appendChar(text, ',');
    strutil::poolIntegers(text, 3, speedX, speedY, enemy);
    return text;
}
void Fireball::LoadAscii(char* str, int* index) {
    Entity::LoadAscii(str, index);
    speedX = strutil::parseInt(str, index);
    speedY = strutil::parseInt(str, index);
    enemy = strutil::parseBool(str, index);
}