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
        // Examples
        else if (id == 11 && triggerID == 3)
            soundService->FadeVolumeMusic(0.2f, 0.5f);
        else if (id == 11 && triggerID == 4)
            window->SetDialogueText("The door is marked by four flame-like symbols.");
        else if (id == 13 && triggerID == 1 && !map->Puzzles[1].Solved)
            window->SetDialogueText("The torch is cold, and is marked with a glyph shaped like a weight.", 75);
        else if (id == 13 && triggerID == 4 && !map->Puzzles[0].Solved)
            window->SetDialogueText("The only thing that seems to be able to reach that crate is a well-aimed fireball.");
        else if (id == 14 && triggerID == 1) {
            soundService->SetVolumeMusic(0.75f);
            soundService->PlayMusic("Audio/Resources/IntoTheCastle.wav", 1);
            soundService->QueueMusic(23.0f, "Audio/Resources/CastleHalls.wav");
            window->ToggleStatusBar(true);
        }
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
        default: producedThought = false; // No default case
    }

    if (producedThought) soundService->PlaySound("Audio/Resources/Think.wav");
}
void Trigger_PuzzleInput(RenderWindow* window, SoundService* SoundService, Particle* particles, Map* map, Entity* entities[]) {

    if (map->GetMapID() == 2) {
        if (map->PressurePlatesPressed == 1) {
            map->FillRectangle(37, 1, 38, 14, TileID::ET_None);
            window->SetDialogueText("As the pressure plate clicks, the wall slides away into the shifting sands below.", 100);
        }
    } else if (map->GetMapID() == 3) {
        if (map->PressurePlatesPressed == 1) {
            map->FillRectangle(37, 1, 38, 14, TileID::ET_None);
        } else {
            map->FillRectangle(37, 1, 38, 14, TileID::ET_Door_Vertical);
        }
    } else if (map->GetMapID() == 4) {
        if (map->PressurePlatesPressed == 2) {
            map->FillRectangle(27, 1, 28, 14, TileID::ET_None);
            map->Triggers[0] = true;
        } else if (map->PressurePlatesPressed == 1 && !map->Triggers[0]) {
            window->SetDialogueText("Perhaps its time to put the sheep to work. Rest or rally them with spacebar.", 0);
        }
    }

    /*
    if (map->GetMapID() == 1) {
        if (map->PressurePlatesPressed == 1) {
            for (int i = 0; i < MaxEntities; i++) {
                Entity* a = entities[i];
                if (a && a->OnPressurePlate && a->GetID() == EntityID::EE_Sheep && !a->Paused)
                    window->SetDialogueText("Make your sheep rest by swinging your staff over them (spacebar).");
            }
        } else if (map->PressurePlatesPressed >= 1) map->PuzzleStatus = true;

        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
    } else if (map->GetMapID() == 3) {
        Entity* lever = GetEntityOccurence(entities, 7, 1);
        if (lever) {
            Lever* l = dynamic_cast<Lever*>(lever);
            if (l) {
                l->ToggleLock(!(map->PressurePlatesPressed >= 1)); // Unlock lever if pressure plate is pressed
                Particle* clickEffect = ActivateParticle(particles, 3, lever->x, lever->y);
                if (map->PressurePlatesPressed >= 1)
                    window->SetDialogueText("You hear a click from the internals of the lever, and its lock has disappeared.");
            }
        }

        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);
    } else if (map->GetMapID() == 9) {
        Trigger_Internal_DisplayPuzzleStatus_Torch(GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities), map->PuzzleStatus);

    } else if (map->GetMapID() == 13) {
        Entity* obj = GetEntityOccurence(entities, EntityID::EE_Torch, 1, MaxEntities);
        if (obj) {
            Torch* torch = dynamic_cast<Torch*>(obj);
            if (torch) {
                torch->glow = map->Puzzles[0].Solved; // Inform the player on whether the main puzzle was solved or not
                torch->HasFire = map->Puzzles[1].Solved; // Light torch if enabled
            }
        }

    } else if (map->GetMapID() == 14 && map->PressurePlatesPressed >= 1) {
        window->SetDialogueText("The pressure plate clicks, and you hear a shifting of stones somewhere nearby.");
    
    } else if (map->GetMapID() == 15) {
        Entity* solution = GetEntityOccurence(entities, EntityID::EE_Torch, 3, MaxEntities);

        Entity* lever1 = GetEntityOccurence(entities, EntityID::EE_Lever, 1, MaxEntities);
        Entity* lever2 = GetEntityOccurence(entities, EntityID::EE_Lever, 2, MaxEntities);
        Entity* lever3 = GetEntityOccurence(entities, EntityID::EE_Lever, 3, MaxEntities);
        Entity* lever4 = GetEntityOccurence(entities, EntityID::EE_Lever, 4, MaxEntities);
        if (lever1 && lever2 && lever3 && lever4) {
            Lever* l1 = dynamic_cast<Lever*>(lever1);
            Lever* l2 = dynamic_cast<Lever*>(lever2);
            Lever* l3 = dynamic_cast<Lever*>(lever3);
            Lever* l4 = dynamic_cast<Lever*>(lever4);

            if (l1 && l2 && l3 && l4) {
                map->Puzzles[0].entities[0]->HasFire = l2->IsFlipped() || l4->IsFlipped();
                map->Puzzles[0].entities[1]->HasFire = l1->IsFlipped() && !l3->IsFlipped();
                map->Puzzles[0].entities[2]->HasFire = !l2->IsFlipped() && !l3->IsFlipped();
                map->Puzzles[0].entities[3]->HasFire = (l3->IsFlipped() || l2->IsFlipped()) || (!l3->IsFlipped() && !l2->IsFlipped());
            } else printf("Levers failed to be cast\n");
        } else printf("Failed to locate all four levers\n");

        Puzzle_CheckSolution(&map->Puzzles[0]);
        Trigger_Internal_DisplayPuzzleStatus_Torch(solution, map->Puzzles[0].Solved);
    }
    */

}
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* world[WorldWidth][WorldHeight], Map* map, Entity* entities[]) {

    /*
    if (map->GetMapID() == 11) {    // Pyramid Gateway
        int DoorRequirements = 0;

        Entity* torch1Obj = GetEntityAtLocation(entities, 9, 4);
        Entity* torch2Obj = GetEntityAtLocation(entities, 9, 10);
        Entity* torch3Obj = GetEntityAtLocation(entities, 15, 4);
        Entity* torch4Obj = GetEntityAtLocation(entities, 15, 10);

        Trigger_Internal_TorchSetup(torch1Obj);
        Trigger_Internal_TorchSetup(torch2Obj);
        Trigger_Internal_TorchSetup(torch3Obj);
        Trigger_Internal_TorchSetup(torch4Obj);

        if (torch1Obj) {
            torch1Obj->HasFire = world[0][2]->PuzzleStatus;
            if (torch1Obj->HasFire) DoorRequirements++;
        }
        if (torch2Obj) {
           torch2Obj->HasFire = world[0][0]->PuzzleStatus;
            if (torch2Obj->HasFire) DoorRequirements++; 
        }
        if (torch3Obj) {
            torch3Obj->HasFire = world[2][2]->PuzzleStatus;
            if (torch3Obj->HasFire) DoorRequirements++;
        }
        if (torch4Obj) {
            torch4Obj->HasFire = world[2][0]->PuzzleStatus;
            if (torch4Obj->HasFire) DoorRequirements++;
        }

        #ifdef DEBUG_MODE
            if (DEBUG_SkipGates >= 1)
                DoorRequirements = 4;
        #endif



        if (DoorRequirements == 4) {    //If all requirements are met, open the door
            map->FillRectangle(26, 5, 27, 10, TileID::ET_None);
            map->FillRectangle(27, 6, 40, 9, TileID::ET_None);
            map->FillRectangle(35, 6, 36, 9, TileID::ET_Trigger3);
            map->FillRectangle(36, 6, 37, 9, TileID::ET_Trigger2);
        }
    } else if (map->GetMapID() == 13) {
        Entity* crate = GetEntityOccurence(world[4][1]->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable);
        if (crate && crate->y == 3) map->FillRectangle(7, 7, 8, 10, TileID::ET_Empty_Puzzle_Piece);
        else map->FillRectangle(7, 7, 8, 10, TileID::ET_Wall);
    }
    */

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

    if (map->GetMapID() == 3) { // Set up puzzle on Start3
        p->Enabled = true; // Enable use of puzzle
        p->entities[0] = GetEntityOccurence(map->StoredEntities, EntityID::EE_Crate, 1, MaxEntitiesStoreable); // The crate is part of this puzzle
        p->PlatesPressed = 1; // We want 1 pressure plate to be pressed by the entities inside this puzzle
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