#pragma once

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
        // Returns the entity ID
        virtual int GetID();
        // Returns the amount of health the entity has
        virtual int GetHealth();
        // Call this function when unloading the entity (storing into map)
        virtual void Unload();

        int x;
        int y;

        // Flips sprite horizontally while rendering
        bool Flipped = false;
        // True if object is on fire
        bool HasFire = false;
        // True if we do not want this entity's default AI to tick
        bool Paused = false;
        // Do we want this object to cause collisions?
        bool Solid = true;

        // Current animation playing--0 is always idle, animations are defined per-object inside the RenderWindow class
        int animation = 0;

        // If true, entity can be saved to or removed from levels upon entering and leaving
        bool archivable = false;
};