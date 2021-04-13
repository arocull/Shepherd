#pragma once

#include <cstdio>

#include "Core/config.h"
#include "Core/enums.h"
#include "Core/mathutil.h"
#include "Entities/entity.h"
#include "Map/map.h"
#include "Entities/movement.h"

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

        bool enteredStun = false;
        bool exitedStun = true;

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
        
        bool ShouldBridge();
        bool ShouldSmash();
        void BuildBridge(Map* map);
        void Smash(Map* map, Entity** entities);
        void ThrowEntity(Entity* hit, Entity** entities, Map* map);

        void SetTarget(Entity* newTarget);
        Entity* GetTarget();
};