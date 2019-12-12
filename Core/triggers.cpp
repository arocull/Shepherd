#include "Core/triggers.h"

void DoMapTrigger(RenderWindow* window, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 3));

    
    if (!map->Triggers[triggerID]) {
        printf("Running trigger %i for map ID %i\n", triggerID, id);

        map->Triggers[triggerID] = true;
        if (id == 0 && triggerID == 1)
            window->SetDialogueText("Your sheep will follow you around as you\nmove. Try to keep track of all of them.", 100);
        else if (id == 0 && triggerID == 2)
            window->SetDialogueText("You need all of your sheep gathered nearby\nbefore you can exit an area.", 100);
        else if (id == 4 && triggerID == 1)
            window->SetDialogueText("Careful around that wolf...!\nWolves like to eat sheep!");
        else if (id == 4 && triggerID == 2)
            window->SetDialogueText("It seems to be sleeping...\nYou can deal with it with a swift\nswing of your staff (spacebar).",100);
        else if (id == 5 && triggerID == 1)
            window->SetDialogueText("Ambush!\nProtect your sheep!");
        else if (id == 8 && triggerID == 1)
            window->SetDialogueText("Congratulations on a job well done.\nTutorial complete!",100);
    }
}