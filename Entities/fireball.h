#pragma once

#include "entity.h"

#include "config.h"

class Fireball : public Entity {
    public:
        Fireball(int spawnX, int spawnY, int dirX, int dirY, int type);

        int speedX;
        int speedY;

        int fireType;
};