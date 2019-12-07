// Entity Management Function Definitions //
#include "entity_management.h"

void AppendEntity(Entity* entities[MaxEntities], Entity* newEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (!entities[i]) {            //Empty slot in list. Go ahead and add them in!
            entities[i] = newEntity;
            return;
        }
    }
}


void AppendEntityDetailed(Entity* entities[MaxEntities], Entity* newEntity, int dataRange, int filter) {
    if (!newEntity || newEntity->GetID() != filter) return;                 //Don't attempt to add something that doesn't exist!

    for (int i = 0; i < dataRange; i++) {
        if (entities[i] == newEntity)       //Entity already exists within list! Don't add them again!
            return;
        else if (!entities[i]) {            //Empty slot in list. Go ahead and add them in!
            entities[i] = newEntity;
            return;
        }
    }
}


void RemoveEntity(Entity* entities[MaxEntities], Entity* delEntity) {
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && delEntity && entities[i] == delEntity) {
            delete entities[i];
            entities[i] = nullptr;
            return;
        }
    }
}


void CleanEntities(Entity* entities[MaxEntities]) {
    for (int i = MaxEntities-1; i > 0; i--) {
        if (!entities[i-1] && entities[i]) {
            entities[i-1] = entities[i];
            entities[i] = nullptr;
        }
    }
}


Entity* GetEntityAtLocation(Entity* entities[MaxEntities], int xPos, int yPos) {
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i] && entities[i]->x == xPos && entities[i]->y == yPos)
            return entities[i];
    }

    return nullptr;
}


bool HasAllSheep(Entity* entities[MaxEntities], Entity* obj) {
    if (!DEBUG_RequireSheep) return true;

    Entity* touchedSheep[MaxSheep+1];
    for (int i = 1; i <= MaxSheep; i++) {   //Initialize list
        touchedSheep[i] = nullptr;
    }
    touchedSheep[0] = obj;

    int sheep = 0;
    
    for (int i = 0; i <= MaxSheep; i++) {
        if (touchedSheep[i]) {
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x+1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y+1),  MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x-1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y-1),  MaxSheep+1, 2);
            if (touchedSheep[i]->GetID() == 2)
                sheep++;
        }
    }

    //printf("Sheep tally is %i sheep\n", sheep);

    return sheep >= MaxSheep;
}