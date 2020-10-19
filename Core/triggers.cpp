#include "Core/triggers.h"

void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 4));

    
    if (triggerID == 4 || !map->Triggers[triggerID - 1]) {
        if (triggerID <= 3)
            map->Triggers[triggerID - 1] = true;
        
        // Starting Area
        if (id == 1 && triggerID == 1)
            window->SetDialogueText("Remember to have all your sheep gathered nearby before you leave.", 0);
        else if (id == 5 && triggerID == 1)
            window->SetDialogueText("You peer over the pitfall into the ravine. You don't remember falling in.", 100);
        else if (id == 6 && triggerID == 1)
            window->SetDialogueText("The lever is covered in the sandy dust of ages. Perhaps a good swing of your staff will brush it off...", 0);
        else if (id == 8 && triggerID == 1)
            window->SetDialogueText("You sigh with relief as you climb out of the sandstone ravine. The sheep may survive after all.", 100);
        else if (id == 8 && triggerID == 2)
            window->SetDialogueText("A large pyramid looms overhead. The gate is covered in strange glyphs.", 100);
        else if (id == 8 && triggerID == 3)
            window->SetDialogueText("The torches are identified with unique glyphs. Stone moulding runs into the sand below them, disappearing from view.", 120);
        else if (id == 13 && triggerID == 1) {
            window->SetDialogueText("The air is dry, but cool. Your footsteps echo around the walls of the corridor as you march your flock along.", 100);
            soundService->FadeVolumeMusic(0.0f, 1.0f);
        }

        /* Examples
        if (id == 14 && triggerID == 1) {
            soundService->SetVolumeMusic(0.75f);
            soundService->PlayMusic("Audio/Resources/IntoTheCastle.wav", 1);
            soundService->QueueMusic(23.0f, "Audio/Resources/CastleHalls.wav");
            window->ToggleStatusBar(true);
        }*/
    }
}
void Trigger_OnScroll(RenderWindow* window, SoundService* soundService, Map* map,  Entity* entities[]) {
    if (!map->HasScroll()) return;

    window->SetDialogueText(map->GetScroll(), 100);
    map->ScrollDiscovered = true;
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

    soundService->PlayMusic("Audio/Resources/AmbientWind.wav");
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
    soundService->PlayMusic("Audio/Resources/AmbientWind.wav");
    soundService->SetVolumeMusic(0);
    soundService->FadeVolumeMusic(1.0f, 3.0f);
}
void Trigger_StaffSwing(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {

    // On starting area, if the player has not done so yet, instruct them on how to move
    if (map->GetMapID() == 1 && (entities[0] && entities[0]->Paused == true)) {
        entities[0]->Paused = false;
        window->SetDialogueText("Use WASD or Arrow Keys to move around.", 0);
        soundService->FadeVolumeMusic(0.2f, 1.0f);
    }
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

    if (producedThought) soundService->PlaySound("Audio/Resources/Think.wav");
}
void Trigger_PuzzleInput(RenderWindow* window, SoundService* SoundService, Particle* particles, Map* map, Entity* entities[]) {

    // THE RAVINE / TUTORIAL //
    if (map->GetMapID() == 2) {
        #ifdef DEBUG_MODE
            if (map->PressurePlatesPressed == 1 || DEBUG_SkipGates >= 1) {
        #else
            if (map->PressurePlatesPressed == 1) {
        #endif
            map->FillRectangle(37, 1, 38, 14, TileID::ET_None);
            window->SetDialogueText("As the pressure plate clicks, the wall slides away into the shifting sands below.", 100);
        }
    } else if (map->GetMapID() == 3) {
        #ifdef DEBUG_MODE
            if (map->PressurePlatesPressed == 1 || DEBUG_SkipGates >= 1) {
        #else
            if (map->PressurePlatesPressed == 1) {
        #endif
            map->FillRectangle(37, 1, 38, 14, TileID::ET_None);
        } else {
            map->FillRectangle(37, 1, 38, 14, TileID::ET_Door_Vertical);
        }
    } else if (map->GetMapID() == 4) {
        #ifdef DEBUG_MODE
            if (map->PressurePlatesPressed == 2 || DEBUG_SkipGates >= 1) {
        #else
            if (map->PressurePlatesPressed == 2) {
        #endif
            map->FillRectangle(27, 1, 28, 14, TileID::ET_None);
            map->Triggers[0] = true;
        } else if (map->PressurePlatesPressed == 1 && !map->Triggers[0]) {
            window->SetDialogueText("Perhaps its time to put the sheep to work. Rest or rally them with spacebar.", 0);
        }

    // THE DESERT //
    } else if (map->GetMapID() == 6) { // Flip lever to complete puzzle
        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
    } else if (map->GetMapID() == 7) { // Step on 3 pressure plates to complete puzzle
        if (map->PressurePlatesPressed == 3) {
            Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), true);
            map->PuzzleStatus = true;
        }
    } else if (map->GetMapID() == 9) { // Hold pressure plate down to lower wall, then push box, retrieve sheep 
        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
        
        Entity* obj = GetEntityAtLocation(entities, 25, 7, MaxEntities); // Get entity on pressure plate (if there is one)
        if (obj && obj->OnPressurePlate) { // If an entity is standing on the pressure plate, lower the wall
            map->FillRectangle(20, 6, 21, 9, TileID::ET_Empty_Puzzle_Piece);
        } else { // Otherwise raise it
            map->FillRectangle(20, 6, 21, 9, TileID::ET_Wall);
        }
    } else if (map->GetMapID() == 10) { // Step on 6 pressure plates to complete puzzle
        if (map->PressurePlatesPressed == 6) {
            Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), true);
            map->PuzzleStatus = true;
        }
    } else if (map->GetMapID() == 11) { // Slide crate to complete puzzle
        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
    }


    // THE PYRAMID //
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
    }

    map->HasLoaded = true;
}




bool Trigger_Internal_CheckAllCrates(Entity* entities[], Map* map, int NumberOfEntities) {
    for (int i = 0; i < NumberOfEntities; i++) {
        if (entities[i] && entities[i]->GetID() == 5 && map->GetTileID(entities[i]->x, entities[i]->y) != 8)
            return false;
    }

    return true;
}
Torch* Trigger_Internal_TorchSetup(Entity* torch, bool extinguishable, bool useable, bool glow, bool hasFire, bool hasFrost) {
    if (!torch) return nullptr;

    Torch* t = dynamic_cast<Torch*>(torch);
    if (t) {
        t->Extinguishable = extinguishable;
        t->FireUsable = useable;
        t->glow = glow;
        t->HasFire = hasFire;
        t->HasFrost = hasFrost;
    }

    return t;
}
Crate* Trigger_Internal_CrateSetup(Entity* crate, bool canIncinerate) {
    if (!crate) return nullptr;

    Crate* c = dynamic_cast<Crate*>(crate);
    if (c) {
        c->canIncinerate = canIncinerate;
    }

    return c;
}
void Trigger_Internal_DisplayPuzzleStatus_Torch(Entity* torch, bool puzzleStatus) {
    if (torch) {
        Torch* t = dynamic_cast<Torch*>(torch);
        if (t) {
            t->Extinguishable = false;
            t->FireUsable = false;
            t->HasFire = puzzleStatus;
            t->glow = puzzleStatus;
            t->UpdateAnimationData();
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
    }

    /*
    if (map->GetMapID() == 1) {
        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
    } if (map->GetMapID() == 3) {
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Lever, 1, MaxEntitiesStoreable);
        p->LeversFlipped = 1;

        if (p->entities[0]) {
            Lever* l = dynamic_cast<Lever*>(p->entities[0]);
            if (l) l->ToggleLock(true);
        }

        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
    } else if (map->GetMapID() == 7) {
        p->Enabled = true;

        Entity* torch1 = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable);
        Entity* torch2 = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 2, MaxEntitiesStoreable);

        Trigger_Internal_TorchSetup(torch1, false, true, false, true, false);
        if (torch2) torch2->HasFire = false;
        
        p->entities[0] = torch2;
    } else if (map->GetMapID() == 9) {
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 1;

        Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable), false, false, false, false, false);
    } else if (map->GetMapID() == 13) {
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        Trigger_Internal_CrateSetup(p->entities[0], true);
        p->PlatesPressed = 0; // First crate needs to be incinerated

        Puzzle* p2 = &(map->Puzzles[1]);
        p2->Enabled = true;
        p2->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 2, MaxEntitiesStoreable);
        p2->PlatesPressed = 1; // Second crate must be pushed onto button

        Entity* torch = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable);
        Trigger_Internal_TorchSetup(torch, false, true, false, false, false);
    } else if (map->GetMapID() == 15) {
        for (int i = 1; i < 6; i++) {
            Trigger_Internal_TorchSetup(GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, i, MaxEntitiesStoreable), false, false, false, false, false);
        }
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 1, MaxEntitiesStoreable);
        p->entities[1] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 2, MaxEntitiesStoreable);
        p->entities[2] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 4, MaxEntitiesStoreable);
        p->entities[3] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Torch, 5, MaxEntitiesStoreable);
        p->FireType = 1;
    }
    */
}