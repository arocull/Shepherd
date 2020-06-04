#include "entity.h"

Entity::Entity() {
    x = MapHeight/2;
    y = MapWidth/2;
    lastX = x;
    lastY = y;
}
Entity::Entity(int xPos, int yPos, int ID) {
    x = xPos;
    y = yPos;
    lastX = xPos;
    lastY = yPos;
    id = ID;
}

int Entity::GetID(){
    return id;
}
int Entity::GetHealth() {
    return Health;
}
void Entity::Unload() {
    
}
void Entity::Tick() {

}
bool Entity::TakeDamage(int dmgAmount, Entity* attacker) {
    Health-=dmgAmount;

    return Health <= 0;
}