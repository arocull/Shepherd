#pragma once

#include "Core/config.h"
#include "Core/enums.h"
#include "Core/renderwindow.h"
#include "Audio/sound_service.h"
#include "Entities/entity.h"
#include "Map/map.h"
#include "Entities/entity_management.h"

namespace Script {
    // Array of function pointers for OnTile trigger scripts
    extern void (**list_OnTile)(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID);

    void Init_OnTile(int largestMapID);
    void Free_OnTile();

    void OnTile_None(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map1(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map5(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map6(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map8(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map13(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map17(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
    void OnTile_Map29(RenderWindow* a, SoundService* b, Map* c, Entity* d[], int e);
}