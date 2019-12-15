#pragma once

#include "Entities/entity.h"

class Wolf : public Entity {
    public:
        Wolf(int x, int y);

    private:
        Entity* target;
        int stun = 0;

    public:
        virtual void Unload() override;

        bool InStun();
        void TickStun();

        void InitiateHunt(Entity* newTarget);
        void EndHunt();
        bool IsHunting();

        Entity* GetTarget();
};