#pragma once

#include "Core/config.h"
#include "Core/enums.h"

#include "Core/renderwindow.h"
#include "Audio/sound_service.h"
#include "Entities/particle.h"

#include "Entities/entity.h"
#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/crate.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/entity_management.h"
#include "Map/map.h"

#include "Triggers/trigger_util.h"

namespace Script {
    typedef void PuzzleInputFunc (RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);

    extern PuzzleInputFunc** list_PuzzleInput;

    void Init_PuzzleInput(int largestMapID);
    void Free_PuzzleInput();

    void PuzzleInput_None(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_GlowFirstTorch(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);

    // Tutorial
    void PuzzleInput_Map2(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map3(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map4(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);

    // Desert
    void PuzzleInput_Map6(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map7(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map9(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map10(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);

    // Pyramid
    void PuzzleInput_Map14(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map17(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map20(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map21(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map23(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
    void PuzzleInput_Map24(RenderWindow* a, SoundService* b, Particle* c, Map* d, Entity* e[]);
}