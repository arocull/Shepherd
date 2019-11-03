#include "fireball.h"

Fireball::Fireball(int spawnX, int spawnY, int dirX, int dirY, int type) {
    id = 3;
    x = spawnX;
    y = spawnY;
    speedX = dirX*FireballSpeed;
    speedY = dirY*FireballSpeed;
    fireType = type;
}