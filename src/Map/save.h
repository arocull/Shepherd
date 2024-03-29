#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <filesystem>

#include "Core/enums.h"
#include "Core/strutils.h"
#include "Core/gamedata.h"
#include "Map/map.h"
#include "Entities/entity.h"
#include "Entities/entity_management.h"

// TODO: Header file that includes all Entity subclasses
#include "Entities/Subclasses/shepherd.h"
#include "Entities/Subclasses/sheep.h"
#include "Entities/Subclasses/fireball.h"
#include "Entities/Subclasses/wolf.h"
#include "Entities/Subclasses/crate.h"
#include "Entities/Subclasses/torch.h"
#include "Entities/Subclasses/lever.h"
#include "Entities/Subclasses/spirit.h"

// FStream stuff referenced from https://www.cplusplus.com/doc/tutorial/files/

namespace SaveLoad {
    using std::ios;
    using std::ofstream;
    using std::fstream;
    using std::string;
    //using std::filesystem;


    // UTILITIES //

    // Creates a new Entity object using the given ID and position
    Entity* NewEntityFromID(EntityID id, int x = 0, int y = 0);
    // Creates a new Entity using an Ascii buffer
    Entity* NewEntityFromAscii(char* buffer);
    // Applies the given Ascii buffer to game data
    void StateFromAscii(char* buffer, GameData* data);


    // LOADING //

    // Loads an entire game, returns true if successful, false otherwise
    bool LoadGame(GameData* data);
    // Loads a given file in as a new map
    Map* LoadMapFile(const char* filePath);
    // Load flags
    void LoadObjects(fstream* file, GameData* data, Map* map, Entity** entities, int maxEntities = MaxEntitiesStoreable);


    // SAVING //

    // Checks to see if the current existing save is valid, returns true if so
    bool SaveGameValid();
    // Saves the game state and all maps
    bool Save(GameData* data);
    // Saves the current game state. Mostly focuses on existing entities, player position, loaded area
    bool SaveState(GameData* data);
    // Saves a given map into a file
    bool SaveMap(Map* map, int x, int y);
};