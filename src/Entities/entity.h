#pragma once

#include "Core/config.h"
#include "Core/enums.h"
#include "SDL2/SDL.h"

#include <Core/strutils.h>

// Entity - Any mobile object with individual properties.
class Entity {
    public:
        Entity();
        Entity(int xPos, int yPos, EntityID ID);

    protected:
        EntityID id = EntityID::EE_None;
        int MaxHealth = 1;
        int Health = 1;

    public:
        // Returns the entity ID
        EntityID GetID();
        // Returns the amount of health the entity has
        int GetHealth();
        // Call this function when unloading the entity (storing into map)
        virtual void Unload();
        // Call this function every tick (handles internal entity functions)
        virtual void Tick();
        // Call this function every tick after most functionality is performed, solidifies values like Fire vs Frost
        virtual void PostTick();
        // Call this function to pause an entity and set it to its proper animation
        virtual void Pause();

        // Called when the entity takes damage; returns true if killed
        virtual bool TakeDamage(int dmgAmount, Entity* attacker);


        // RENDERING //
        
        // Updates lastX and lastY to the current Entity location if not shoved, ticks shoved otherwise
        virtual void ShoveAnimation();
        // Gets flip style of entity for rendering
        SDL_RendererFlip GetFlipStyle();
        // Resets entity animation timers
        void ResetAnimationTimers();
        // Draws the entity
        virtual void Draw(SDL_Renderer* canvas, SDL_Texture* texture, SDL_Rect* tile, float delta);


        // DATA
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
        AnimationID animation = AnimationID::ANIM_Idle;
        // Animation timer in game ticks
        unsigned int animationTimerTicks = 0;
        // Animation timer in real seconds
        float animationTimerTime = 0.0f;

        // If true, entity can be saved to or removed from levels upon entering and leaving
        bool archivable = true;

        // Generates an Ascii string that contains data on this entity
        virtual std::string* Ascii();
        // Loads data from a given ascii string, using an index pointer that is at the first number in the string.
        // - Assume the ID has already been passed
        virtual void LoadAscii(char* str, int* index);
};