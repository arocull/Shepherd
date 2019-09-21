#include "Core/config.h"

class Entity {
    public:
        Entity();
        Entity(int xPos, int yPos, int ID);

    private:
        int id = 0;

    public:
        int GetID();

        int x;
        int y;
};