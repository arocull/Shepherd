#include "entity.h"

Entity::Entity() {
    x = MapHeight/2;
    y = MapWidth/2;
}
Entity::Entity(int xPos, int yPos, int ID) {
    x = xPos;
    y = yPos;
    id = ID;
}

int Entity::GetID(){
    return id;
}
int Entity::GetHealth() {
    return Health;
}