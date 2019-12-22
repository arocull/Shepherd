#include "Core/triggers.h"

void DoMapTrigger(RenderWindow* window, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 4));

    
    if (!map->Triggers[triggerID]) {
        //printf("Running trigger %i for map ID %i\n", triggerID, id);

        if (triggerID <= 3)
            map->Triggers[triggerID] = true;

        // Staring Area
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
    }
}