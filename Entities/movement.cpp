// Movement //

#include "Entities/movement.h"

bool MoveUp = false;
bool MoveDown = false;
bool MoveRight = false;
bool MoveLeft = false;
bool Move_QueueClear = false;
bool MoveFireballQueued = false;

void Movement_Clear() {
    MoveUp = false;
    MoveDown = false;
    MoveRight = false;
    MoveLeft = false;
    Move_QueueClear = false;
}


void Movement_ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    int distX = abs(dx);
    int distY = abs(dy);

    int xChange = sgn(dx);
    int yChange = -sgn(dy);

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    //A* pathing?
    
    if (distY > distX) {
        for (int stepY = 0; stepY < distY; stepY++) {
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->IsTileSolid(obj->x, obj->y+yChange) || GetEntityAtLocation(entities, obj->x, obj->y+yChange))
                break;
            else
                obj->y+=yChange;
        }
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->IsTileSolid(obj->x+xChange, obj->y) || GetEntityAtLocation(entities, obj->x+xChange, obj->y))
                break;
            else
                obj->x+=xChange;
        }
    } else {
        for (int stepX = 0; stepX < distX; stepX++) {
            if (obj->x+xChange < 0 || obj->x+xChange >= MapWidth || world->IsTileSolid(obj->x+xChange, obj->y) || GetEntityAtLocation(entities, obj->x+xChange, obj->y))
                break;
            else
                obj->x+=xChange;
        }
        for (int stepY = 0; stepY < distY; stepY++) {
            if (obj->y+yChange < 0 || obj->y+yChange >= MapHeight || world->IsTileSolid(obj->x, obj->y+yChange) || GetEntityAtLocation(entities, obj->x, obj->y+yChange))
                break;
            else
                obj->y+=yChange;
        }
    }

    return;
}


void Movement_ShiftPlayer(Map* world, Entity* entities[MaxEntities], Shepherd* obj, int dx, int dy, int* worldX, int* worldY) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    obj->lastX = dx;
    obj->lastY = dy;
    obj->animation = 1;     //Set animation to walking

    if (desiredX >= MapWidth && *worldX+1 <= WorldWidth-1 && HasAllSheep(entities, obj)) {
        *worldX = *worldX+1;
        obj->x = 0;
    } else if (desiredX < 0 && *worldX-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldX = *worldX-1;
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !world->IsTileSolid(desiredX, obj->y)) {
        Entity* hit = GetEntityAtLocation(entities, desiredX, obj->y);
        if (hit && hit->GetID() == 2) {   //If shep, swap places
            hit->x = obj->x;
            obj->Flipped = !obj->Flipped;
        }

        obj->x = desiredX;
    }
    
    if (desiredY < 0 && *worldY+1 <= WorldHeight-1 && HasAllSheep(entities, obj)) {
        *worldY = *worldY+1;
        obj->y = MapHeight-1;
    } else if (desiredY >= MapHeight && *worldY-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldY = *worldY-1;
        obj->y = 0;
    } else if (desiredY < MapHeight && desiredY >= 0 && !world->IsTileSolid(obj->x, desiredY)) {
        Entity* hit = GetEntityAtLocation(entities, obj->x, desiredY);
        if (hit && hit->GetID() == 2)   //If shep, swap places
            hit->y = obj->y;
            
        obj->y = desiredY;
    }
    
    return;
}