#include "Core/config.h"

// Entity - Any mobile object with individual properties.
class Entity {
    public:
        Entity();
        Entity(int xPos, int yPos, int ID);

    protected:
        int id = 0;
        int MaxHealth = 1;
        int Health = 1;

    public:
        int GetID();
        int GetHealth();

        int x;
        int y;

        bool Flipped = false;
};