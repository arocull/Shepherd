#pragma once

#include <cstdio>

#include "Core/config.h"
#include "Entities/entity.h"
#include "Core/enums.h"
#include "Core/mathutil.h"

class PyramidGolem : public Entity {
    public:
        PyramidGolem(int x, int y);

    private:
        Entity* target = nullptr;
        int stun = 0;

        int otherTick = false;
        int fireballTimer;
        bool tossFireball = false;

        int startY;
        int goalMoveX = 0;
        int goalMoveY = 0;

    public:
        virtual void Unload() override;
        virtual void Tick() override;

        int GetGoalX();
        int GetGoalY();
        bool DoFireballToss();
        void UpdateFireVisual();

        virtual bool TakeDamage(int dmgAmount, Entity* attacker) override;

        bool InStun();
        void TickStun();

        void SetTarget(Entity* newTarget);
        Entity* GetTarget();
};