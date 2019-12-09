#include "Core/triggers.h"

void DoMapTrigger(RenderWindow* window, Map* map, Entity* entities[], int triggerID) {
    int id = map->GetMapID();

    triggerID = max(1,min(triggerID, 3));

    
    if (id == 0 && !map->Triggers[triggerID]) {
        printf("Running trigger %i for map ID %i\n", triggerID, id);

        map->Triggers[triggerID] = true;
        if (triggerID == 1)
            window->SetDialogueText("Your sheep will follow you around as you\nmove. Try to keep track of all of them.", 100);
        else if (triggerID == 2)
            window->SetDialogueText("You need all of your sheep gathered nearby\nbefore you can exit an area.", 100);
    }
}