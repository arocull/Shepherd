#pragma once

#include "Entities/entity.h"

#include "Entities/particle.h"


// Crate - A box that can be pushed around to solve various puzzles
class Crate : public Entity {
    public:
        Crate(int xPos, int yPos);

        // Crate type--can be used to create sliding puzzles where different crates have different destinations
        int type = 0;
        bool canIncinerate = false;

        void Incinerate(Particle* particles, Entity* attacker);

        // Generates an Ascii string that contains data on this entity. See Entity::Ascii
        virtual std::string* Ascii() override;
        // Loads data from a given ascii string. See Entity::LoadAscii
        virtual void LoadAscii(char* str, int* index) override;
};