#include "Core/triggers.h"

void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 4));

    
    if (!map->Triggers[triggerID]) {
        if (triggerID <= 3)
            map->Triggers[triggerID] = true;

        
        // Staring Area
        if (id == 1 && triggerID == 1)
            window->SetDialogueText("Pressure plates can be weighed down by standing on them.");
        else if (id == 3 && triggerID == 1)
            window->SetDialogueText("Levers can be flipped by swinging your staff near them, but need to be unlocked first.", 75);
        else if (id == 5 && triggerID == 1)
            window->SetDialogueText("Your sheep will always follow you. Try to keep track of all of them.", 75);
        else if (id == 5 && triggerID == 2)
            window->SetDialogueText("You cannot leave an area without all of your sheep gathered around you.", 75);
        else if (id == 7 && triggerID == 1)
            window->SetDialogueText("Swing your staff while near torche to pick up their flames.", 75);
        else if (id == 7 && triggerID == 4 && entities[0])  // Extinguish player's flame when leaving
            entities[0]->HasFire = false;
        else if (id == 9 && triggerID == 1)
            window->SetDialogueText("You can push crates around by walking into them.", 75);
        else if (id == 9 && triggerID == 2)
            window->SetDialogueText("Crates cannot be pushed off of surfaces.");
        else if (id == 11 && triggerID == 1)
            window->SetDialogueText("These torches are covered in odd glyphs, but you sense they might be related to the nearby ruins.", 125);
        else if (id == 11 && triggerID == 4)
            window->SetDialogueText("The door is marked by four flame-like symbols.");
    }
}
void Trigger_GameStart(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    // Starting Level Cinematic
    for (int i = 0; i < MaxEntities; i++) {
        Entity* ent = entities[i];
        if (ent) {
            ent->Paused = true;
            if (ent->GetID() == 2)
                ent->animation = 1;
            else if (ent->GetID() == 1)
                ent->animation = 3;
        }
    }
    window->SetDialogueText("\nHit spacebar to rally your sheep.", 0);

    soundService->PlaySound("Audio/Resources/AmbientWind.wav");
}
void Trigger_StaffSwing(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {

    // On starting area, if the player has not done so yet, instruct them on how to move
    if (map->GetMapID() == 5 && (map->Triggers[3] == false || (entities[0] && entities[0]->Paused == true))) {
        map->Triggers[3] = true;
        entities[0]->Paused = false;
        window->SetDialogueText("Use WASD or Arrow Keys to move around.", 0);
        soundService->FadeVolume(0.2f, 2.5f);
        window->ToggleStatusBar(true);
    } else if (map->GetMapID() == 7 && (entities[0] && entities[0]->HasFire))
        window->SetDialogueText("Walk towards the unlit torch and swing your staff to toss a fireball.");
}
void Trigger_Idled(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    if (map->GetMapID() == 1)
        window->SetDialogueText("Some puzzles might require a bit of help from your wooly friends.");
    else if (map->GetMapID() == 5 && (entities[0] && !entities[0]->Paused))
        window->SetDialogueText("A hungry sheep bleats timidly. You wonder worriedly on when you will be able to feed them again.", 100);
    else if (map->GetMapID() == 8)
        window->SetDialogueText("Looking forward, a pyramid looms over you. Behind are some mountains, hopefully with greener pastures on the other side.", 100);
    else if (map->GetMapID() == 9)
        window->SetDialogueText("Crates can be pushed by moving into them.", 100);
    else if (map->GetMapID() == 11)
        window->SetDialogueText("The pyramid entrance is somewhat ominous, and you sense a faint, supernatural presence.", 100);
}
void Trigger_PuzzleInput(RenderWindow* window, SoundService* SoundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->GetMapID() == 1 && map->PressurePlatesPressed >= 2)
        map->PuzzleStatus = true;
    else if (map->GetMapID() == 3) {
        Entity* lever = GetEntityOccurence(entities, 7, 1);
        if (lever) {
            Lever* l = dynamic_cast<Lever*>(lever);
            if (l) {
                l->ToggleLock(!(map->PressurePlatesPressed >= 1)); // Unlock lever if pressure plate is pressed
                Particle* clickEffect = ActivateParticle(particles, 3, lever->x, lever->y);
            }
        }
    }
}
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* world[WorldWidth][WorldHeight], Map* map, Entity* entities[]) {

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



        if (DoorRequirements == 4) {    //If all requirements are met, open the door
            map->FillRectangle(26, 5, 27, 10, 0);
            map->FillRectangle(27, 6, 40, 9, 0);
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
Torch* Trigger_Internal_TorchSetup(Entity* torch, bool extinguishable, bool useable, bool glow) {
    if (!torch) return nullptr;

    Torch* t = dynamic_cast<Torch*>(torch);
    if (t) {
        t->Extinguishable = extinguishable;
        t->FireUsable = useable;
        t->glow = glow;
    }

    return t;
}






void Trigger_SetupPuzzles(Map* map) {
    Puzzle* p = &(map->Puzzles[0]);

    if (map->GetMapID() == 3) {
        p->Enabled = true;
        p->entities[0] = GetEntityOccurence(map->StoredEntities, 7, 1, MaxEntitiesStoreable);
        p->LeversFlipped = 1;

        if (p->entities[0]) {
            Lever* l = dynamic_cast<Lever*>(p->entities[0]);
            if (l)
                l->ToggleLock(true);
        }
    } else if (map->GetMapID() == 7) {
        p->Enabled = true;

        Entity* torch1 = GetEntityAtLocation(map->StoredEntities, 8, 4, MaxEntitiesStoreable);
        Entity* torch2 = GetEntityAtLocation(map->StoredEntities, 35, 4, MaxEntitiesStoreable);

        Trigger_Internal_TorchSetup(torch1, false, true, false);
        if (torch2)
            torch2->HasFire = false;
        
        p->entities[0] = torch2;
    } else if (map->GetMapID() == 9) {
        p->Enabled = true;

        p->entities[0] = GetEntityOccurence(map->StoredEntities, 5, 1, MaxEntitiesStoreable);
        p->PlatesPressed = 1;
    }
}