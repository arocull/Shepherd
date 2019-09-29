#include "entity.h"

#include "config.h"

// Shepherd - A character that cares for and commands a flock of sheep. Typically for player.
class Shepherd : public Entity {
    public:
        Shepherd(int spawnX, int spawnY);

    private:

    public:
        bool HasAllSheep();

        int lastX = 0;
        int lastY = 0;
};