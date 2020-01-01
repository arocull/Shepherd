#include "Core/triggers.h"

void Trigger_OnTile(RenderWindow* window, Map* map, Entity* entities[], int triggerID) {
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
    }
}
void Trigger_GameStart(RenderWindow* window, Map* map, Entity* entities[]) {
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
}
void Trigger_StaffSwing(RenderWindow* window, Map* map, Entity* entities[]) {

    // On starting area, if the player has not done so yet, instruct them on how to move
    if (map->GetMapID() == 5 && map->Triggers[3] == false) {
        map->Triggers[3] == true;
        entities[0]->Paused = false;
        window->SetDialogueText("Use WASD or Arrow Keys to move around.", 0);
    }
}
void Trigger_LevelLoaded(RenderWindow* window, Map* map, Entity* entities[]) {

}



/* Tutorial Triggers
        if (id == 0 && triggerID == 1)
            window->SetDialogueText("Your sheep will follow you around as you\nmove. Try to keep track of all of them.", 75);
        else if (id == 0 && triggerID == 2)
            window->SetDialogueText("You need all of your sheep gathered nearby\nbefore you can exit an area.", 75);

        else if (id == 1 && triggerID == 1)
            window->SetDialogueText("You can singal for your sheep to rest \n(spacebar) while near them. You can also\nre-rally them(spacebar again).", 75);

        // Sleeping Wolf Area
        else if (id == 4 && triggerID == 4) {       // Force all wolves in map to sleep
            for (int i = 0; i < MaxEntities; i++) {
                if (entities[i] && entities[i]->GetID() == 4) {
                    entities[i]->Paused = true;
                    entities[i]->animation = 3;
                }
            }
        } else if (id == 4 && triggerID == 1)
            window->SetDialogueText("Careful around that wolf...!\nWolves like to eat sheep!");
        else if (id == 4 && triggerID == 2)
            window->SetDialogueText("It seems to be sleeping...\nYou can deal with it with a swift swing\nor two of your staff (spacebar).",100);

        // Wolf Ambush Area
        else if (id == 5 && triggerID == 1)
            window->SetDialogueText("Ambush!\nProtect your sheep!");

        // Tutorial End
        else if (id == 8 && triggerID == 1)
            window->SetDialogueText("Congratulations on a job well done.\nTutorial complete!",100);
*/