#include "Triggers/triggers.h"


void Trigger_Init(int maxMapID) {
    Script::Init_OnTile(maxMapID);
    Script::Init_PuzzleInput(maxMapID);
}
void Trigger_Free() {
    Script::Free_OnTile();
    Script::Free_PuzzleInput();
}


void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    triggerID = max(1,min(triggerID, 4));
    
    if (triggerID == 4 || !map->Triggers[triggerID - 1]) {
        if (triggerID <= 3)
            map->Triggers[triggerID - 1] = true;
        
        (Script::list_OnTile[map->GetMapID()])(window, soundService, map, entities, triggerID);
    }
}
void Trigger_OnScroll(RenderWindow* window, SoundService* soundService, Map* map,  Entity* entities[]) {
    if (!map->HasScroll()) return;

    window->SetDialogueText(map->GetScroll(), 100);
    map->ScrollDiscovered = true;
}
void Trigger_OnFizzler(RenderWindow* window, SoundService* soundService, Map* map,  Entity* shepherd) {
    if (shepherd && (shepherd->HasFire || shepherd->HasFrost)) {
        shepherd->HasFire = false;
        shepherd->HasFrost = false;
        window->SetDialogueText("With a quiet hiss, the glowing flames on your staff fizzle out into a fine, swirling smoke.");
    }
}
void Trigger_GameStart(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    // Starting Level Cinematic
    for (int i = 0; i < MaxEntities; i++) {
        Entity* ent = entities[i];
        if (ent) {
            ent->Paused = true;
            if (ent->GetID() == EntityID::EE_Sheep)
                ent->animation = 1;
            else if (ent->GetID() == EntityID::EE_Shepherd)
                ent->animation = 3;
        }
    }
    window->SetDialogueText("\nHit spacebar to rally your sheep.", 0);

    #ifdef DEBUG_MODE
        if (DEBUG_SkipGates < 2) {
            soundService->PlayMusic("Assets/Audio/AmbientWind.wav");
        }
    #else
        soundService->PlayMusic("Assets/Audio/AmbientWind.wav");
    #endif
}
void Trigger_GameOver(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    // Starting Level Cinematic
    for (int i = 0; i < MaxEntities; i++) {
        Entity* ent = entities[i];
        if (ent) {
            ent->Paused = true;
            ent->lastX = ent->x;
            ent->lastY = ent->y;
            ent->shovedX = 0;
            ent->shovedY = 0;
            switch (ent->GetID()) {
                case EntityID::EE_Sheep: ent->animation = 1; break;
                case EntityID::EE_Wolf: ent->animation = 2; break;
                case EntityID::EE_Shepherd: ent->animation = 3; break;
            }
        }
    }
    window->SetDialogueText("\nLet the tears flow.", 0);

    soundService->StopAllSounds();
    soundService->PlayMusic("Assets/Audio/AmbientWind.wav");
    soundService->SetVolumeMusic(0);
    soundService->FadeVolumeMusic(1.0f, 3.0f);
}
void Trigger_StaffSwing(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    if (!entities[0]) return; // Return if there is no shepherd

    // On starting area, if the player has not done so yet, instruct them on how to move
    if (map->GetMapID() == 1 && entities[0]->Paused == true) {
        window->SetDialogueText("Use WASD or Arrow Keys to move around.", 0);
        soundService->FadeVolumeMusic(0.2f, 1.0f);
    
    // Pyramid - Teach player how to throw fireballs
    } else if (map->GetMapID() == 17) {
        if (entities[0]->HasFire) window->SetDialogueText("Swing your staff again to toss a fireball.", 0);
        else if (!map->PuzzleStatus) window->SetDialogueText("Fire from torches can be picked up with a swing of your staff.", 0);
    }

    entities[0]->Paused = false; // Unpause Shepherd whenever staff is swung
}
void Trigger_Idled(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    if ((entities[0] && entities[0]->Paused)) return;
    bool producedThought = true;

    switch (map->GetMapID()) {
        case 1: window->SetDialogueText("An opening lies in the east wall on your right. Perhaps it should be investigated.", 0); break;
        case 2: window->SetDialogueText("The stray tile in the sand looks like it could be stood upon.", 0); break;
        case 3: window->SetDialogueText("The box looks like it could be pushed along the arrow from left to right.", 0); break;
        case 12:
            window->SetDialogueText("A wall slides out of the pyramid and into the sand, revealing a dusty tomb.", 0);
            map->FillRectangle(39, 6, 40, 12, TileID::ET_None);
        default: producedThought = false; // No default case
    }

    if (producedThought) soundService->PlaySound("Assets/Audio/Think.wav");
}
void Trigger_PuzzleInput(RenderWindow* window, SoundService* soundService, Particle* particles, Map* map, Entity* entities[]) {
    (Script::list_PuzzleInput[map->GetMapID()])(window, soundService, particles, map, entities);
}
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* world[WorldWidth][WorldHeight], Map* map, Entity* entities[]) {

    // Trigger puzzles immeadietly to allow quick gate-skipping
    #ifdef DEBUG_MODE
        if (map->GetMapID() <= 4 && DEBUG_SkipGates >= 1) {
            map->PressurePlatesPressed = 1;
        }
    #endif

    if (map->GetMapID() == 8) { // Desert Gate
        Entity* torch1 = GetEntityOccurence(entities, EE_Torch, 1, MaxEntities); // Get torches
        Entity* torch2 = GetEntityOccurence(entities, EE_Torch, 2, MaxEntities);
        Entity* torch3 = GetEntityOccurence(entities, EE_Torch, 3, MaxEntities);
        Entity* torch4 = GetEntityOccurence(entities, EE_Torch, 4, MaxEntities);
        Entity* torch5 = GetEntityOccurence(entities, EE_Torch, 5, MaxEntities);

        Trigger_Internal_DisplayPuzzleStatus_Torch(torch1, world[1][1]->PuzzleStatus); // Display what puzzles are complete
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch2, world[2][1]->PuzzleStatus);
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch3, world[0][0]->PuzzleStatus);
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch4, world[1][0]->PuzzleStatus);
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch5, world[2][0]->PuzzleStatus);

        #ifdef DEBUG_MODE
            if ((torch1->HasFire && torch2->HasFire && torch3->HasFire && torch4->HasFire && torch5->HasFire) || DEBUG_SkipGates >= 2) {
        #else
            if (torch1->HasFire && torch2->HasFire && torch3->HasFire && torch4->HasFire && torch5->HasFire) { // Open gate if all puzzles are triggered
        #endif
            map->FillRectangle(37, 6, 39, 11, TileID::ET_None);
            map->FillRectangle(39, 6, 40, 11, TileID::ET_Empty_Puzzle_Piece);
        }
    } else if (map->GetMapID() == 18) { // Pyramid Gates
        Entity* torch1 = GetEntityOccurence(entities, EE_Torch, 1, MaxEntities); // Get torches
        Entity* torch2 = GetEntityOccurence(entities, EE_Torch, 2, MaxEntities);
        Entity* torch3 = GetEntityOccurence(entities, EE_Torch, 3, MaxEntities);
        Entity* torch4 = GetEntityOccurence(entities, EE_Torch, 4, MaxEntities);

        Trigger_Internal_DisplayPuzzleStatus_Torch(torch1, world[5][2]->PuzzleStatus); // Display what puzzles are complete
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch2, world[7][2]->PuzzleStatus);
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch3, world[5][0]->PuzzleStatus);
        Trigger_Internal_DisplayPuzzleStatus_Torch(torch4, world[7][0]->PuzzleStatus);

        #ifdef DEBUG_MODE
            if ((torch1->HasFire && torch2->HasFire) || DEBUG_SkipGates >= 3) {
        #else
            if (torch1->HasFire && torch2->HasFire) {
        #endif
            map->FillRectangle(35, 6, 36, 9, TileID::ET_None);
        } else {
            map->FillRectangle(35, 6, 36, 9, TileID::ET_Door_Vertical);
        }
        if (torch3->HasFire && torch4->HasFire) {
            map->FillRectangle(17, 2, 24, 3, TileID::ET_None);
        } else {
            map->FillRectangle(17, 2, 24, 3, TileID::ET_Door_Horizontal);
        }
    } else if (map->GetMapID() == 22) { // Bottom right corner of pyramid, open wall if all current puzzles are solved
        bool gatesOpen = world[5][2]->PuzzleStatus && world[7][2]->PuzzleStatus && world[5][0]->PuzzleStatus && world[7][0]->PuzzleStatus;
        if (gatesOpen) map->FillRectangle(39, 6, 40, 9, TileID::ET_None);
        else map->FillRectangle(39, 6, 40, 9, TileID::ET_Wall);
    }

    map->HasLoaded = true;
}


void Trigger_LevelEvent(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    int triggerID = map->GetEventID();

    if (map->GetMapID() == 24 && !map->PuzzleStatus) {
        Entity* torch = GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities);
        AppendEntity(entities, new Fireball(torch->x - 1, torch->y, -1, 0, 1));
        map->SetEventTimer(24, 0); // 20 Ticks (2.5 seconds) before next fireball spawn
    } else if (map->GetMapID() == 29) {
        if (triggerID == 0) {
            window->SetDialogueText("...Panic?", 0);
            map->SetEventTimer(24, 1);
        } else if (triggerID == 1) {
            window->SetDialogueText("Panic!", 30);
            window->AddScreenShake(1.0f, 0);
            map->FillRectangle(2, 1, 38, 4, TileID::ET_None);
            PyramidGolem* boss = new PyramidGolem(20,2);
            boss->SetTarget(GetEntityOccurence(entities, EntityID::EE_Shepherd, 1, MaxEntities));
            AppendEntity(entities, boss);
        }
    }
}


void Trigger_SetupPuzzles(Map* map) {
    Puzzle* p = &(map->Puzzles[0]);

    if (map->GetMapID() <= 14) { // Disable all torches in starting area
        for (int i = 0; i < MaxEntitiesStoreable; i++) {
            if (map->StoredEntities[i] && map->StoredEntities[i]->GetID() == EntityID::EE_Torch) { // Make sure entity exists and has the right ID
                Trigger_Internal_TorchSetup(map->StoredEntities[i], false, false, false, false, false); // Turns torch off and makes it unignitable
            }
        }
    }

    // DESERT //
    if (map->GetMapID() == 3) { // Set up puzzle on Start3
        p->Enabled = true; // Enable use of puzzle
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable); // The crate is part of this puzzle
        p->PlatesPressed = 1; // We want 1 pressure plate to be pressed by the entities inside this puzzle
    } else if (map->GetMapID() == 6) { // Lever puzzle on Desert 2
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Lever, 1, MaxEntitiesStoreable); // This time we just want to use this lever
        p->LeversFlipped = 1; // All the player needs to do is flip it!
    } else if (map->GetMapID() == 9) {
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 1; // Ultimately, we want the crate on the pressure plate
    } else if (map->GetMapID() == 11) { // Just another sliding box puzzle on Desert 6, this time with a curve
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 1;

    // PYRAMID //
    } else if (map->GetMapID() == 14) { // Top Left Puzzle Indicator + Crates
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 2, MaxEntitiesStoreable);
        p->PlatesPressed = 1;

        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
        Trigger_Internal_CrateSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable), true);
        Trigger_Internal_CrateSetup(p->entities[0], false);
    } else if (map->GetMapID() == 15) { // Top middle torch for carrying fire
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, true, false, true, false);
    } else if (map->GetMapID() == 16) { // Top Right Puzzle Indicator + Lever
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Lever, 1, MaxEntitiesStoreable);
        p->LeversFlipped = 1;

        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
    } else if (map->GetMapID() == 17) { // Pyramid Entrance
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable);
        p->entities[1] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 3, MaxEntitiesStoreable);
        p->entities[0]->HasFire = false;
        p->entities[1]->HasFire = false;
        p->FireType = 1;

        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 2, MaxEntitiesStoreable), false, true, false, true, false);
    } else if (map->GetMapID() == 18) { // Pyramid Gates Area
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 2, MaxEntitiesStoreable), false, false, false, false, false);
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 3, MaxEntitiesStoreable), false, false, false, false, false);
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 4, MaxEntitiesStoreable), false, false, false, false, false);
    } else if (map->GetMapID() == 20) { // Bottom Left--Crate incineration puzzle
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 0;
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, true, false, true, false);
        Trigger_Internal_CrateSetup(p->entities[0], true);
    } else if (map->GetMapID() == 22) { // Bottom Right - Pushing multiple crates at once
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->entities[1] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 2, MaxEntitiesStoreable);
        p->PlatesPressed = 2;
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
    } else if (map->GetMapID() == 23) { // Fireball catching training
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 2, MaxEntitiesStoreable);
        Trigger_Internal_TorchSetup(p->entities[0], true, true, false, false, false);
    } else if (map->GetMapID() == 24) {
        map->SetEventTimer(12, 0); // Show fireball quickly so player understands what's going on
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 0;
        // Both crates need to be burnable
        Trigger_Internal_CrateSetup(p->entities[0], true);
        Trigger_Internal_CrateSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 2, MaxEntitiesStoreable), true);
    }
}