#include "Core/triggers.h"

void Trigger_OnTile(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 4));

    
    if (!map->Triggers[triggerID]) {
        if (triggerID <= 3)
            map->Triggers[triggerID] = true;

        
        // Staring Area
        if (id == 5 && triggerID == 1)
            window->SetDialogueText("Your sheep will always follow you.\nTry to keep track of all of them.", 75);
        else if (id == 5 && triggerID == 2)
            window->SetDialogueText("You cannot leave an area without\nall of your sheep gathered around you.", 75);
        else if (id == 9 && triggerID == 1)
            window->SetDialogueText("You can push crates around by walking\ninto them. Crates, like you and your sheep,\nweigh down pressure plates.", 80);
        else if (id == 9 && triggerID == 2)
            window->SetDialogueText("Crates cannot be pushed off of surfaces.", 50);
        else if (id == 11 && triggerID == 1)
            window->SetDialogueText("These torches are covered in odd glyphs,\nbut you sense they might be related\nto the nearby ruins.", 125);
        else if (id == 11 && triggerID ==2)
            window->SetDialogueText("The glyphs on the door are from an unknown\nand ancient language.", 75);
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
    if (map->GetMapID() == 5 && (map->Triggers[3] == false || (entities[0] && entities[0]->Paused == false))) {
        map->Triggers[3] == true;
        entities[0]->Paused = false;
        window->SetDialogueText("Use WASD or Arrow Keys to move around.", 0);
        soundService->FadeVolume(0.2f, 2.5f);
    }
}
void Trigger_Idled(RenderWindow* window, SoundService* soundService, Map* map, Entity* entities[]) {
    if (map->GetMapID() == 5 && (entities[0] && !entities[0]->Paused))
        window->SetDialogueText("A hungry sheep bleats timidly. You wonder\nworriedly on when you will be able to\nfeed them again.", 100);
    else if (map->GetMapID() == 8)
        window->SetDialogueText("Looking forward, a pyramid looms over you.\nBehind are some mountains, hopefully\nwith greener pastures on the other side.", 100);
    else if (map->GetMapID() == 9)
        window->SetDialogueText("Crates can be pushed by moving into them.", 100);
    else if (map->GetMapID() == 11)
        window->SetDialogueText("The pyramid entrance is somewhat ominous,\nand you sense a faint, supernatural presence.", 100);
}
void Trigger_PuzzleInput(RenderWindow* window, SoundService* SoundService, Particle* particles, Map* map, Entity* entities[]) {
    if (map->GetMapID() == 9 && Trigger_Internal_CheckAllCrates(entities, map) && entities[0])
        ActivateParticle(particles, 2, entities[0]->x, entities[0]->y);
}
void Trigger_LevelLoaded(RenderWindow* window, SoundService* soundService, Map* world[WorldWidth][WorldHeight], Map* map, Entity* entities[]) {
    
    
    if (map->GetMapID() == 11) {    // Pyramid Gateway
        int DoorRequirements = 0;

        if (Trigger_Internal_CheckAllCrates(world[2][0]->StoredEntities, world[2][0], MaxEntitiesStoreable)) {
            map->SetTile(15, 10, 4);
            DoorRequirements++;
        } else
            map->SetTile(15, 10, 6);

        if (DoorRequirements == 4) {    //If all requirements are met, open the door
            map->FillRectangle(26, 5, 27, 10, 0);
            map->FillRectangle(27, 6, 28, 9, 0);
        }
    }
}




bool Trigger_Internal_CheckAllCrates(Entity* entities[], Map* map, int NumberOfEntities) {
    for (int i = 0; i < NumberOfEntities; i++) {
        if (entities[i] && entities[i]->GetID() == 5 && map->GetTileID(entities[i]->x, entities[i]->y) != 8)
            return false;
    }

    return true;
}

/* Tutorial Triggers
        // Sleeping Wolf Area
        if (id == 4 && triggerID == 4) {       // Force all wolves in map to sleep
            for (int i = 0; i < MaxEntities; i++) {
                if (entities[i] && entities[i]->GetID() == 4) {
                    entities[i]->Paused = true;
                    entities[i]->animation = 3;
                }
            }
        }
*/