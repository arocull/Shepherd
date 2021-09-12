#include "Entities/movement.h"

bool Movement::MoveUp = false;
bool Movement::MoveDown = false;
bool Movement::MoveRight = false;
bool Movement::MoveLeft = false;
bool Movement::Move_QueueClear = false;
bool Movement::MoveFireballQueued = false;

void Movement::ClearQueue() {
    MoveUp = false;
    MoveDown = false;
    MoveRight = false;
    MoveLeft = false;
    Move_QueueClear = false;
}

bool Movement::CheckCollision(Map* world, Entity* entities[MaxEntities], int x, int y) {
    return !(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        world->IsTileSolid(x, y) ||
        (EntityTools::GetEntityAtLocation(entities, x, y) && EntityTools::GetEntityAtLocation(entities, x, y)->Solid)
    );
}
bool Movement::CheckCollisionWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy) {
    if (!(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        world->IsTileSolid(x, y)
    )) {
        bool pushed = true; // Default to true, true if no entity or entity was moved off of tile

        Entity* a = EntityTools::GetEntityAtLocation(entities, x, y);
        if (a && EntityTools::GetEntityAtLocation(entities, x, y)->Solid) {
            pushed = false; // Entity is here and solid
            switch (a->GetID()) {
                case EntityID::EE_Sheep:
                    pushed = Movement::ShiftEntity(world, entities, a, dx, dy, true); // True if entity moved, false if they were not
                    break;
                case EntityID::EE_Crate:
                    pushed = Movement::ShiftEntityOnTiles(world, entities, a, dx, dy, TileID::ET_Empty_Puzzle_Piece, true);
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
bool Movement::CheckCollisionOnTiles(Map* world, Entity* entities[MaxEntities], int x, int y, int tileID) {
    return !(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        (world->GetTileID(x, y) != tileID && world->GetTileID(x, y) != TileID::ET_Pressure_Plate) ||
        (EntityTools::GetEntityAtLocation(entities, x, y) && EntityTools::GetEntityAtLocation(entities, x, y)->Solid)
    );
}
bool Movement::CheckCollisionOnTilesWithPush(Map* world, Entity* entities[MaxEntities], int x, int y, int dx, int dy, int tileID) {
    if (!(
        x < 0 ||
        x >= MapWidth ||
        y < 0 ||
        y >= MapHeight ||
        (world->GetTileID(x, y) != tileID && world->GetTileID(x, y) != TileID::ET_Pressure_Plate) // Make sure this is the tile we're looking for, or a pressure plate
    )) {
        bool pushed = true; // Default to true, true if no entity or entity was moved off of tile

        Entity* a = EntityTools::GetEntityAtLocation(entities, x, y);
        if (a && EntityTools::GetEntityAtLocation(entities, x, y)->Solid) {
            pushed = false; // Entity is here and solid
            switch (a->GetID()) {
                case EntityID::EE_Sheep:
                    pushed = Movement::ShiftEntity(world, entities, a, dx, dy, true); // True if entity moved, false if they were not
                    break;
                case EntityID::EE_Crate:
                    pushed = Movement::ShiftEntityOnTiles(world, entities, a, dx, dy, tileID, true);
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



bool Movement::ShiftEntity(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, bool doPush) {
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
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionWithPush(world, entities, obj->x, obj->y+yChange, dx, dy); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionWithPush(world, entities, obj->x+xChange, obj->y, dx, dy); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionWithPush(world, entities, obj->x+xChange, obj->y, dx, dy); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionWithPush(world, entities, obj->x, obj->y+yChange, dx, dy); stepY++)
                obj->y+=yChange;
        }
    } else {
        if (distY > distX) {
            for (int stepY = 0; stepY < distY && Movement::CheckCollision(world, entities, obj->x, obj->y+yChange); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement::CheckCollision(world, entities, obj->x+xChange, obj->y); stepX++)
                obj->x+=xChange;
        } else {
            for (int stepX = 0; stepX < distX && Movement::CheckCollision(world, entities, obj->x+xChange, obj->y); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement::CheckCollision(world, entities, obj->x, obj->y+yChange); stepY++)
                obj->y+=yChange;
        }
    }

    return (obj->x == desiredX && obj->y == desiredY);
}

bool Movement::ShiftEntityOnTiles(Map* world, Entity* entities[MaxEntities], Entity* obj, int dx, int dy, int tileID, bool doPush) {
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

    if (doPush) { // If we can push entities...
        if (distY > distX) { // If we're moving more vertically than horizontally, prioritize vertical movement first
            // For every vertical step, make sure the tile is pathable, push entities out of the way, and then move there
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionOnTilesWithPush(world, entities, obj->x, obj->y+yChange, dx, dy, tileID); stepY++)
                obj->y+=yChange;
            // Same as above, but for horizontal steps
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionOnTilesWithPush(world, entities, obj->x+xChange, obj->y, dx, dy, tileID); stepX++)
                obj->x+=xChange;
        } else { // Otherwise, prioritize horizontal movement rather than vertical
            // Same as above
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionOnTilesWithPush(world, entities, obj->x+xChange, obj->y, dx, dy, tileID); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionOnTilesWithPush(world, entities, obj->x, obj->y+yChange, dx, dy, tileID); stepY++)
                obj->y+=yChange;
        }
    } else { // If we can't push entities, just move forward
        if (distY > distX) { // Prioritize Y movement first
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionOnTiles(world, entities, obj->x, obj->y+yChange, tileID); stepY++)
                obj->y+=yChange;
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionOnTiles(world, entities, obj->x+xChange, obj->y, tileID); stepX++)
                obj->x+=xChange;
        } else { // Prioritize X movement first
            for (int stepX = 0; stepX < distX && Movement::CheckCollisionOnTiles(world, entities, obj->x+xChange, obj->y, tileID); stepX++)
                obj->x+=xChange;
            for (int stepY = 0; stepY < distY && Movement::CheckCollisionOnTiles(world, entities, obj->x, obj->y+yChange, tileID); stepY++)
                obj->y+=yChange;
        }
    }

    return (obj->x == desiredX && obj->y == desiredY); // Return true if we ended up at the desired location, false otherwise
}


void Movement::ShiftPlayer(GameData* data, int dx, int dy) {
    Shepherd* obj = data->player;
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

    if (desiredX >= MapWidth && data->worldX+1 <= WorldWidth-1 && EntityTools::HasAllSheep(data->entities, obj)) {
        data->worldX = data->worldX+1;
        obj->x = 0;
    } else if (desiredX < 0 && data->worldX-1 >= 0 && EntityTools::HasAllSheep(data->entities, obj)) {
        data->worldX = data->worldX-1;
        obj->x = MapWidth-1;
    } else if (desiredX < MapWidth && desiredX >= 0 && !data->map->IsTileSolid(desiredX, obj->y) && !data->map->IsTilePitfall(desiredX, obj->y)) {
        Entity* hit = EntityTools::GetEntityAtLocation(data->entities, desiredX, obj->y);
        if (hit && hit->GetID() == EntityID::EE_Sheep) {   //If sheep, swap places
            hit->lastX = hit->x;
            hit->shovedX = true;
            hit->x = obj->x;
            obj->x = desiredX;
            obj->Flipped = !obj->Flipped;
        } else if (hit && hit->GetID() == EntityID::EE_Crate) {  //If hit crate, push crate, then move to place if push was successful
            hit->lastX = hit->x;
            hit->shovedX = true;
            if (Movement::ShiftEntityOnTiles(data->map, data->entities, hit, dx, dy, TileID::ET_Empty_Puzzle_Piece, true))
                obj->x = desiredX;
        } else if (!hit || (hit && !hit->Solid))
            obj->x = desiredX;
    }
    
    if (desiredY < 0 && data->worldY+1 <= WorldHeight-1 && EntityTools::HasAllSheep(data->entities, obj)) {
        data->worldY = data->worldY+1;
        obj->y = MapHeight-1;
    } else if (desiredY >= MapHeight && data->worldY-1 >= 0 && EntityTools::HasAllSheep(data->entities, obj)) {
        data->worldY = data->worldY-1;
        obj->y = 0;
    } else if (desiredY < MapHeight && desiredY >= 0 && !data->map->IsTileSolid(obj->x, desiredY) && !data->map->IsTilePitfall(obj->x, desiredY)) {
        Entity* hit = EntityTools::GetEntityAtLocation(data->entities, obj->x, desiredY);
        if (hit && hit->GetID() == EntityID::EE_Sheep) {   //If sheep, swap places
            hit->lastY = hit->y;
            hit->shovedY = true;
            hit->y = obj->y;
            obj->y = desiredY;
        } else if (hit && hit->GetID() == EntityID::EE_Crate) {  //If hit crate, push crate, then move to place if push was successful
            hit->lastY = hit->y;
            hit->shovedY = true;
            if (Movement::ShiftEntityOnTiles(data->map, data->entities, hit, dx, dy, TileID::ET_Empty_Puzzle_Piece, true))
                obj->y = desiredY;
        } else if (!hit || (hit && !hit->Solid))
            obj->y = desiredY;
    }
    
    return;
}