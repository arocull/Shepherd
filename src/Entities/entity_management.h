/* Entity Management

General utilities for managing lists of entities
*/

#pragma once

#include <stdlib.h>

#include "mathutil.h"

#include "Entities/entity.h"

namespace EntityTools {
    // Adds the selected entity to the given list of entities
    void AppendEntity(Entity* entities[MaxEntities], Entity* newEntity, int arrayLength = MaxEntities);

    /*  Adds the selected entity to the given list of entities

    Also filters out null data and prevents doubling up on things, while also having a defined range and a set filter*/
    void AppendEntityDetailed(Entity* entities[MaxEntities], Entity* newEntity, int dataRange, int filter);


    // Removed the selected entity from the given list of entities
    void RemoveEntity(Entity* entities[MaxEntities], Entity* delEntity);
    // Prune the list of entities
    void CleanEntities(Entity* entities[MaxEntities]);

    // Searches list of entities and returns the first entity it finds with given location; otherwise returns null
    Entity* GetEntityAtLocation(Entity* entities[MaxEntities], int xPos, int yPos, int dataRange = MaxEntities);
    // Returns the Nth occurrence of an entity of a given type in an array of entities; returns null if not present
    Entity* GetEntityOccurence(Entity* entities[MaxEntities], int id, int occurrence = 1, int dataRange = MaxEntities);

    // Finds the entity with the nearest location, excluding anyone at the given location
    Entity* GetNearestEntity(Entity* entities[MaxEntities], int xPos, int yPos);
    // Finds the entity with the given ID that is closest to the location, excluding anyone at the given location
    Entity* GetNearestEntity(Entity* entities[MaxEntities], int xPos, int yPos, int ArraySize, int searchID);

    /* Checks to make sure all sheep are touching the shepherd (or touching other sheep touching the shepherd)

    Warning - Has many iterations and can cause a lot of overhead. Use sparingly*/
    bool HasAllSheep(Entity* entities[MaxEntities], Entity* obj);
}