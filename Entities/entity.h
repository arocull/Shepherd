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
        int GetID();
        // Returns the amount of health the entity has
        int GetHealth();
        // Call this function when unloading the entity (storing into map)
        virtual void Unload();
        // Call this function every tick (handles internal entity functions)
        virtual void Tick();

        // Called when the entity takes damage; returns true if killed
        virtual bool TakeDamage(int dmgAmount, Entity* attacker);


        // Horizontal position of Entity
        int x;
        // Vertical position of Entity
        int y;

        // Previous horizontal position of Entity
        int lastX = 0;
        // Previous vertical position of entity
        int lastY = 0;
        // True if the entity was pushed around by the player horizontally
        bool shovedX = false;
        // True if the entity was pushed around by the player vertically
        bool shovedY = false;

        // Flips sprite horizontally while rendering
        bool Flipped = false;
        // True if object is on fire
        bool HasFire = false;
        // True if object has frostflame
        bool HasFrost = false;
        // True if we do not want this entity's default AI to tick
        bool Paused = false;
        // Do we want this object to cause collisions?
        bool Solid = true;
        // Is this object ontop of a pressure plate?
        bool OnPressurePlate = false;

        // Current animation playing--0 is always idle, animations are defined per-object inside the RenderWindow class
        int animation = 0;
        // Animation Metadata; entity-specific data for the renderer to interpret when drawing entities or animations
        int animationMetadata = 0;

        // If true, entity can be saved to or removed from levels upon entering and leaving
        bool archivable = false;
};