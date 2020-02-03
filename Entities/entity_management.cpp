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
        if (entities[i]) {
            if (entities[i]->GetHealth() <= 0 && entities[i]->GetID() != 1) {
                delete entities[i];
                entities[i] = nullptr;

            } else if (!entities[i-1]) {
                entities[i-1] = entities[i];
                entities[i] = nullptr;
            }
        }
    }
}


Entity* GetEntityAtLocation(Entity* entities[MaxEntities], int xPos, int yPos, int dataRange) {
    for (int i = 0; i < dataRange; i++) {
        if (entities[i] && entities[i]->x == xPos && entities[i]->y == yPos)
            return entities[i];
    }

    return nullptr;
}
Entity* GetEntityOccurence(Entity* entities[MaxEntities], int id, int occurrence, int dataRange) {
    occurrence--;

    int count = 0;
    for (int i = 0; i < dataRange; i++) {
        if (entities[i] && entities[i]->GetID() == id) {
            if (count == occurrence)
                return entities[i];
            
            count++;
        }
    }

    return nullptr;
}


Entity* GetNearestEntity(Entity* entities[MaxEntities], int xPos, int yPos) {
    Entity* closest = nullptr;

    int dis = 1000;
    for (int i = 0; i < MaxEntities; i++) {
        if (entities[i]) {
            int d = dist(xPos, yPos, entities[i]->x, entities[i]->y);
            if (d < dis) {
                dis = d;
                closest = entities[i];
            }
        }
    }

    return closest;
}
Entity* GetNearestEntity(Entity* entities[MaxEntities], int xPos, int yPos, int ArraySize, int searchID) {
    Entity* closest = nullptr;

    int dis = 1000;
    for (int i = 0; i < ArraySize; i++) {
        if (entities[i] && entities[i]->GetID() == searchID) {
            int d = dist(xPos, yPos, entities[i]->x, entities[i]->y);
            if (d < dis) {
                dis = d;
                closest = entities[i];
            }
        }
    }

    return closest;
}


bool HasAllSheep(Entity* entities[MaxEntities], Entity* obj) {
    if (!DEBUG_RequireSheep) return true;   //Return if sheep are unnecessary for leaving

    Entity* touchedSheep[MaxSheep+1];   //List holds all sheep + one shepherd
    for (int i = 1; i <= MaxSheep; i++) {   //Initialize list
        touchedSheep[i] = nullptr;
    }
    touchedSheep[0] = obj;  //Count player first and finds any sheep touching him

    int sheep = 0;
    
    // Iterate through list of entities and check all four sides of them for any other entities to add to the list (corners don't count!)
    for (int i = 0; i <= MaxSheep; i++) {
        if (touchedSheep[i]) {
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x+1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y+1),  MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x-1,  touchedSheep[i]->y),    MaxSheep+1, 2);
            AppendEntityDetailed(touchedSheep, GetEntityAtLocation(entities, touchedSheep[i]->x,    touchedSheep[i]->y-1),  MaxSheep+1, 2);
            if (touchedSheep[i]->GetID() == 2)  //If this entity itself is a sheep, tally them
                sheep++;
        }
    }

    return sheep >= MaxSheep;
}