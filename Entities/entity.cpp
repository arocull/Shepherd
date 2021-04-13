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
}
bool Entity::TakeDamage(int dmgAmount, Entity* attacker) {
    Health-=dmgAmount;

    return Health <= 0;
}