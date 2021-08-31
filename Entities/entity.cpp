#include "entity.h"

Entity::Entity() {
    x = MapHeight/2;
    y = MapWidth/2;
    lastX = x;
    lastY = y;
}
Entity::Entity(int xPos, int yPos, EntityID ID) {
    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;
    id = ID;
}

EntityID Entity::GetID(){
    return id;
}
int Entity::GetHealth() {
    return Health;
}
void Entity::Unload() {
    
}
void Entity::Tick() {

}
void Entity::Pause() {
    Paused = true;
    animation = AnimationID::ANIM_Paused;
}
bool Entity::TakeDamage(int dmgAmount, Entity* attacker) {
    Health-=dmgAmount;

    return Health <= 0;
}

SDL_RendererFlip Entity::GetFlipStyle() {
    // Return a horizontal flip if the sprite is flipped--otherwise return no flip
    return Flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}
void Entity::ResetAnimationTimers() {
    animationTimerTicks = 0;
    animationTimerTime = 0.0f;
}
void Entity::Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta) {
    SDL_RenderCopy(canvas, texture, NULL, tile); // Render entire given texture as default
}


std::string* Entity::Ascii() {
    std::string* text = new std::string();

    strAppendChar(text, 'E'); // Signal that this is an Entity
    // Dump important data into a string of integers
    strPoolIntegers(text, 11, id, x, y, MaxHealth, Health, HasFire, HasFrost, Flipped, Paused, animation, archivable);

    return text;
}