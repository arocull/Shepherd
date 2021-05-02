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


bool Movement_CheckCollision(Map* world, Entity* entities[MaxEntities], int x, int y) {
    return !(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        world->IsTileSolid(x, y) ||
        (GetEntityAtLocation(entities, x, y) && GetEntityAtLocation(entities, x, y)->Solid)
    );
}
bool Movement_CheckCollisionWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy) {
    if (!(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        world->IsTileSolid(x, y)
    )) {
        bool pushed = true; // Default to true, true if no entity or entity was moved off of tile

        Entity* a = GetEntityAtLocation(entities, x, y);
        if (a && GetEntityAtLocation(entities, x, y)->Solid) {
            pushed = false; // Entity is here and solid
            switch (a->GetID()) {
                case EntityID::EE_Sheep:
                    pushed = Movement_ShiftEntity(world, entities, a, dx, dy, true); // True if entity moved, false if they were not
                    break;
                case EntityID::EE_Crate:
                    pushed = Movement_ShiftEntityOnTiles(world, entities, a, dx, dy, TileID::ET_Empty_Puzzle_Piece, true);
                    break;
            }

            if (pushed) {
                if (a->x = x + dx) {
                    a->lastX = x;
                    a->shovedX = true;
                }
                if (a->y = y - dy) {
                    a->lastY = y;
                    a->shovedY = true;
                }
            }
        }
        return pushed;
    }

    return false;
}
bool Movement_CheckCollisionOnTiles(Map* world, Entity* entities[MaxEntities], int x, int y, int tileID) {
    return !(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        (world->GetTileID(x, y) != tileID && world->GetTileID(x, y) != 8) ||
        (GetEntityAtLocation(entities, x, y) && GetEntityAtLocation(entities, x, y)->Solid)
    );
}
bool Movement_CheckCollisionOnTilesWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy, int tileID) {
    if (!(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        (world->GetTileID(x, y) != tileID && world->GetTileID(x, y) != 8)
    )) {
        bool pushed = true; // Default to true, true if no entity or entity was moved off of tile

        Entity* a = GetEntityAtLocation(entities, x, y);
        if (a && GetEntityAtLocation(entities, x, y)->Solid) {
            pushed = false; // Entity is here and solid
            switch (a->GetID()) {
                case EntityID::EE_Sheep:
                    pushed = Movement_ShiftEntity(world, entities, a, dx, dy, true); // True if entity moved, false if they were not
                    break;
                case EntityID::EE_Crate:
                    pushed = Movement_ShiftEntityOnTiles(world, entities, a, dx, dy, tileID, true);
                    break;
            }

            if (pushed) {
                if (a->x = x + dx) {
                    a->lastX = x;
                    a->shovedX = true;
                }
                if (a->y = y - dy) {
                    a->lastY = y;
                    a->shovedY = true;
                }
            }
        }
        return pushed;
    }

    return false;
}



bool Movement_ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, bool doPush) {
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


    //Look into A* pathing?
    if (doPush) {
        if (distY > distX) {
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionWithPush(world, entities, obj->x, obj->y+yChange, dx, dy); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionWithPush(world, entities, obj->x+xChange, obj->y, dx, dy); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionWithPush(world, entities, obj->x+xChange, obj->y, dx, dy); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionWithPush(world, entities, obj->x, obj->y+yChange, dx, dy); stepY++)
                obj->y+=yChange;
        }
    } else {
        if (distY > distX) {
            for (int stepY = 0; stepY < distY && Movement_CheckCollision(world, entities, obj->x, obj->y+yChange); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement_CheckCollision(world, entities, obj->x+xChange, obj->y); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement_CheckCollision(world, entities, obj->x+xChange, obj->y); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement_CheckCollision(world, entities, obj->x, obj->y+yChange); stepY++)
                obj->y+=yChange;
        }
    }

    return (obj->x == desiredX && obj->y == desiredY);
}

bool Movement_ShiftEntityOnTiles(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, int tileID, bool doPush) {
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

    if (doPush) {
        if (distY > distX) {
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionOnTilesWithPush(world, entities, obj->x, obj->y+yChange, dx, dy, tileID); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionOnTilesWithPush(world, entities, obj->x+xChange, obj->y, dx, dy, tileID); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionOnTilesWithPush(world, entities, obj->x+xChange, obj->y, dx, dy, tileID); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionOnTilesWithPush(world, entities, obj->x, obj->y+yChange, dx, dy, tileID); stepY++)
                obj->y+=yChange;
        }
    } else {
        if (distY > distX) {
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionOnTiles(world, entities, obj->x, obj->y+yChange, tileID); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionOnTiles(world, entities, obj->x+xChange, obj->y, tileID); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement_CheckCollisionOnTiles(world, entities, obj->x+xChange, obj->y, tileID); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement_CheckCollisionOnTiles(world, entities, obj->x, obj->y+yChange, tileID); stepY++)
                obj->y+=yChange;
        }
    }

    return (obj->x == desiredX && obj->y == desiredY);
}


void Movement_ShiftPlayer(Map* world, Entity* entities[MaxEntities], Shepherd* obj, int dx, int dy, int* worldX, int* worldY) {
    int desiredX = obj->x + dx;
    int desiredY = obj->y - dy;

    if (dx < 0)
        obj->Flipped = true;
    else if (dx > 0)
        obj->Flipped = false;

    obj->faceX = sgn(dx);
    obj->faceY = sgn(dy);
    obj->animation = AnimationID::ANIM_Walk;     //Set animation to walking
    obj->ticksIdled = 0;

    if (desiredX >= MapWidth && *worldX+1 <= WorldWidth-1 && HasAllSheep(entities, obj)) {
        *worldX = *worldX+1;
        obj->x = 0;
    } else if (desiredX < 0 && *worldX-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldX = *worldX-1;
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !world->IsTileSolid(desiredX, obj->y) && !world->IsTilePitfall(desiredX, obj->y)) {
        Entity* hit = GetEntityAtLocation(entities, desiredX, obj->y);
        if (hit && hit->GetID() == EntityID::EE_Sheep) {   //If sheep, swap places
            hit->lastX = hit->x;
            hit->shovedX = true;
            hit->x = obj->x;
            obj->x = desiredX;
            obj->Flipped = !obj->Flipped;
        } else if (hit && hit->GetID() == EntityID::EE_Crate) {  //If hit crate, push crate, then move to place if push was successful
            hit->lastX = hit->x;
            hit->shovedX = true;
            if (Movement_ShiftEntityOnTiles(world, entities, hit, dx, dy, TileID::ET_Empty_Puzzle_Piece, true))
                obj->x = desiredX;
        } else if (!hit || (hit && !hit->Solid))
            obj->x = desiredX;
    }
    
    if (desiredY < 0 && *worldY+1 <= WorldHeight-1 && HasAllSheep(entities, obj)) {
        *worldY = *worldY+1;
        obj->y = MapHeight-1;
    } else if (desiredY >= MapHeight && *worldY-1 >= 0 && HasAllSheep(entities, obj)) {
        *worldY = *worldY-1;
        obj->y = 0;
    } else if (desiredY < MapHeight && desiredY >= 0 && !world->IsTileSolid(obj->x, desiredY) && !world->IsTilePitfall(obj->x, desiredY)) {
        Entity* hit = GetEntityAtLocation(entities, obj->x, desiredY);
        if (hit && hit->GetID() == EntityID::EE_Sheep) {   //If sheep, swap places
            hit->lastY = hit->y;
            hit->shovedY = true;
            hit->y = obj->y;
            obj->y = desiredY;
        } else if (hit && hit->GetID() == EntityID::EE_Crate) {  //If hit crate, push crate, then move to place if push was successful
            hit->lastY = hit->y;
            hit->shovedY = true;
            if (Movement_ShiftEntityOnTiles(world, entities, hit, dx, dy, TileID::ET_Empty_Puzzle_Piece, true))
                obj->y = desiredY;
        } else if (!hit || (hit && !hit->Solid))
            obj->y = desiredY;
    }
    
    return;
}