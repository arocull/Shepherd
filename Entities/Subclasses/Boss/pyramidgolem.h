#pragma once

#include "Entities/entity.h"

#include "Entities/AI/path_manager.h"

class PyramidGolem : public Entity {
    public:
        PyramidGolem(int x, int y);

    private:
        int stun = 0;

    public:
        virtual void Unload() override;

        bool InStun();
        void TickStun();

        Entity* GetTarget();

        // Holds pathdata
        Path* currentPath = nullptr;
};