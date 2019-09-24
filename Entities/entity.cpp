#include "entity.h"

Entity::Entity() {

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