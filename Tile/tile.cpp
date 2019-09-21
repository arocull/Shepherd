#include "tile.h"


Tile::Tile() {
    
}
Tile::Tile(int xPos, int yPos, int tileID = 0) {
    x = xPos;
    y = yPos;
    SetTileID(tileID);
}


int Tile::GetTileID() {
    return id;
}
bool Tile::IsSolid() {
    return solid;
}



void Tile::SetTileID(int newID) {
    id = newID;
    switch (id) {
        case 1:
        case 3:
            solid = true;
            break;
        default:
            solid = false;
    }
}
void Tile::SetPosition(int newX, int newY) {
    x = newX;
    y = newY;
}