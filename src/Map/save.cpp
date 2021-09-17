#include "Map/save.h"

Entity* SaveLoad::NewEntityFromID(EntityID id, int x, int y) {
    switch (id) {
        case EntityID::EE_Sheep:
            return new Sheep(x, y);
        case EntityID::EE_Fireball:
            return new Fireball(x, y, 0, 0, 0);
        case EntityID::EE_Wolf:
            return new Wolf(x, y);
        case EntityID::EE_Crate:
            return new Crate(x, y);
        case EntityID::EE_Torch:
            return new Torch(x, y);
        case EntityID::EE_Lever:
            return new Lever(x, y);
        case EntityID:: EE_Spirit:
            return new Spirit(x, y);
        case EntityID::EE_None:
        default:
            return nullptr;
    }
}
Entity* SaveLoad::NewEntityFromAscii(char* buffer) {
    int idx = 1; // Start one character in, since char 0 is a flag for the Ascii buffer type
    EntityID entityID = (EntityID) strutil::parseInt(buffer, &idx);
    Entity* obj = NewEntityFromID(entityID, 0, 0); // Set current position to 0, 0--will be overridden when loading data
    if (nullptr != obj) {
        obj->LoadAscii(buffer, &idx);
    }

    return obj;
}

Map* SaveLoad::LoadMapFile(const char* filePath) {
    std::fstream mapFile;       //Creates stream to read from
    mapFile.open(filePath);     //Opens file path

    Map* map = new Map();       //Generates new map
    
    // Iterate through the map tiles, row by row 
    for (int y = 0; y < MapHeight && !mapFile.eof(); y++) {
        for (int x = 0; x < MapWidth && !mapFile.eof(); x++) {
            int charID;
            int tileID = TileID::ET_None;
            EntityID entID = EntityID::EE_None;

            charID = mapFile.get(); //Get next character in line

            if (charID == '\n' && !mapFile.eof())         //If char ID is a newline, get the next character instead
                charID = mapFile.get();


            // Check character and set tile ID accordingly; defaults to 0 if none of these are present
            switch (charID) {
                // Entity Spawns - Any character IDs that are not used for tiles are marked as entity-spawning and redirected
                case 'c':       // Crate (with Empty Puzzle Piece tile beneath)
                    tileID = TileID::ET_Empty_Puzzle_Piece;
                    entID = EntityID::EE_Crate;
                    break;
                case 'C':       // Crate (with Pressure Plate tile beneath)
                    tileID = TileID::ET_Pressure_Plate;
                    entID = EntityID::EE_Crate;
                    break;
                case 'h':       // Wolf (or Hound)
                    entID = EntityID::EE_Wolf;
                    break;
                case 't':       // Torch
                    entID = EntityID::EE_Torch;
                    break;
                case 'l':       // Lever
                    entID = EntityID::EE_Lever;
                    break;
                case 'S':       // Spirit
                    entID = EntityID::EE_Spirit;
                    break;

                default:
                    tileID = charID;
                    break;
            }
            if (EntityID::EE_None != entID) {
                Entity* entity = NewEntityFromID(entID, x, y);

                if (entity) {
                    entity->archivable = true;
                    EntityTools::AppendEntity(map->StoredEntities, entity);
                }
            }
            
            SetTileID(&(map->tiles[x][y]), tileID);
        }
    }

    mapFile.get();  // Dump last newline to get to map data
    if (mapFile.eof()) { // RETURN. Todo: Use goto?? Or is that evil?
        mapFile.close();
        return map;
    }
    
    char digit1 = mapFile.get();
    char digit2 = mapFile.get();

    int mapID = ((int) (digit1 - '0')) * 10 + ((int) (digit2 - '0')); // Convert map ID to number
    map->SetMapID(mapID);

    char biome = mapFile.get(); // If there is a biome, get it
    if (mapFile.eof()) { // RETURN
        mapFile.close();
        return map;
    }
    map->SetMapBiome((EnvironmentID) biome); // Cast char to EnvironmentID (for standardization purposes/easier to keep track of)

    // After biomes, the only thing left in standard map data is scroll text, so build a string and set it using that
    mapFile.get();  // Clear out newline
    if (!mapFile.eof()) {   // Make sure we aren't at the end of the file though
        char* scrollName = (char*) calloc(sizeof(char), MaxScrollNameLength);

        int scrollIndex = 0;
        for (; scrollIndex < MaxScrollNameLength - 1 && !mapFile.eof(); scrollIndex++) {
            char currentChar = mapFile.get();
            scrollName[scrollIndex] = currentChar;
            if (currentChar == '\n') { // Stop sampling on newline, character will get replaced with a terminating character
                scrollIndex++;
                break;
            }
        }
        scrollName[scrollIndex - 1] = '\0';     // Terminating character
        scrollIndex = 0; // Reset scroll text index

        // Do not need to grab newline here--is collected within scroll name builder
        if (!mapFile.eof()) {
            char* scroll = (char*) calloc(sizeof(char), MaxScrollLength);
            for (; scrollIndex < MaxScrollLength - 1 && !mapFile.eof(); scrollIndex++) {
                char currentChar = mapFile.get();
                scroll[scrollIndex] = currentChar;
                if (currentChar == '\n') { // Stop sampling on newline, character will get replaced with a terminating character
                    scrollIndex++;
                    break;
                }
            }
            scroll[scrollIndex - 1] = '\0';     // Terminating character

            map->SetScroll(scroll, scrollName);
            free(scroll);
        }
        free(scrollName);
    }

    if (!mapFile.eof()) { // Again, make sure we're not at end of file
        // Now we are getting to save-states. Here, we pull text line-by-line until we 
        // Then, process each line based off of the given flag at the start of the line.
        char* buffer = (char*) calloc(sizeof(char), MaxAsciiLoadBuffer);
        for (int i = 0; i < MaxAsciiLoadBuffer && !mapFile.eof(); i++) {
            mapFile.getline(buffer, MaxAsciiLoadBuffer);
            printf("Map %i loaded string %s\n", map->GetMapID(), buffer);

            int idx = 1;

            switch (buffer[0]) {
                case 'M':
                    map->LoadAscii(buffer, &idx);
                    break;
                case 'E':
                    EntityTools::AppendEntity(map->StoredEntities, SaveLoad::NewEntityFromAscii(buffer), MaxEntitiesStoreable);
                    break;
                case 'P':
                    // TODO: Load puzzle data
                    break;
            }
        }
        free(buffer);
    }

    mapFile.close();
    return map;
}


// SAVING //

bool SaveLoad::Save(GameData* data) {
    // TODO: Check if save folder exists
    // TODO: Multiple saves?
    
    SaveState(data);
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            SaveMap(data->world[x][y], x, y);
        }
    }

    // TODO: Pack saves into one file?

    return true;
}
bool SaveLoad::SaveState(GameData* data) {
    

    return true;
}
bool SaveLoad::SaveMap(Map* map, int x, int y) {
    string* data = map->Ascii();

    string filename = "save/m"; // Indicate this is a map
    filename.append(std::to_string(x)); // X position
    strutil::appendChar(&filename, ','); // Comma
    filename.append(std::to_string(y)); // Y position

    // Push in Entity Data
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (nullptr != map->StoredEntities[i]) {
            Entity* obj = map->StoredEntities[i];
            std::string* entityData = obj->Ascii();

            strutil::appendChar(data, '\n');
            data->append(*entityData);
            delete entityData;
        }
    }

    // TODO: Reconfigure puzzles after loading map and entitites
    for (int i = 0; i < MaxPuzzles; i++) {
        // if (nullptr != map->Puzzles[i]) {
        //     Puzzle* puzzle = map->Puzzles[i];
        // }
    }


    ofstream saveFile;
    saveFile.open(filename.c_str(), ios::out | ios::trunc);

    if (!saveFile.is_open()) {
        perror("Failed to save map file:\n");
        printf("\t%s, with data:\n%s\n", filename.c_str(), data->c_str());
        delete data;
        return false;
    }

    saveFile << data->c_str();

    saveFile.close();

    delete data;
    return true;
}