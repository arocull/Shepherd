#include "Map/save.h"

Entity* SaveLoad::NewEntityFromID(EntityID id, int x, int y) {
    switch (id) {
        case EntityID::EE_Shepherd:
            return new Shepherd(x, y);
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
void SaveLoad::StateFromAscii(char* buffer, GameData* data) {
    if (nullptr == data) {
        return;
    }

    int idx = 1;
    data->ticks = strutil::parseInt(buffer, &idx);
    data->worldX = strutil::parseInt(buffer, &idx);
    data->worldY = strutil::parseInt(buffer, &idx);
}



bool SaveLoad::LoadGame(GameData* data) {
    // First, load all maps
    for (int x = 0; x < WorldWidth; x++) {
        for (int y = 0; y < WorldHeight; y++) {
            string filename = "save/m"; // Build map name
            filename.append(std::to_string(x)); // X position
            strutil::appendChar(&filename, ','); // Comma
            filename.append(std::to_string(y)); // Y position

            data->world[x][y] = LoadMapFile(filename.c_str()); // Load map with given filename
            if (nullptr == data->world[x][y]) { // If loading fails, abort load
                // TODO: Delete all loaded maps when we load a fresh save
                return false;
            }
        }
    }

    // Open main file
    fstream state;
    state.open("save/STATE");
    if (!state.is_open()) {
        return false;
    }

    // Then, load state
    SaveLoad::LoadObjects(&state, data, nullptr, data->entities, MaxEntities);

    for (int i = 0; i < MaxEntities; i++) { // Find the Shepherd and set him as the player
        if (data->entities[i] && data->entities[i]->GetID() == EntityID::EE_Shepherd) {
            data->player = dynamic_cast<Shepherd*>(data->entities[i]);
            break;
        }
    }
    if (nullptr == data->player) { // If no Shepherd was found, this save is invalid
        return false;
    }

    return true; // If no errors occurred, we are good to go!
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

            if (charID == '\n' && !mapFile.eof()) { // If char ID is a newline, get the next character instead
                charID = mapFile.get();
            }


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

    // Load stored objects like entities, puzzles, and map states
    SaveLoad::LoadObjects(&mapFile, nullptr, map, map->StoredEntities, MaxEntitiesStoreable);

    mapFile.close();
    return map;
}
void SaveLoad::LoadObjects(fstream* file, GameData* data, Map* map, Entity** entities, int maxEntities) {
    // Now we are getting to save-states. Here, we pull text line-by-line until we 
    // Then, process each line based off of the given flag at the start of the line.
    char* buffer = (char*) calloc(sizeof(char), MaxAsciiLoadBuffer);
    for (int i = 0; i < MaxAsciiLoadBuffer && !file->eof(); i++) {
        file->getline(buffer, MaxAsciiLoadBuffer);

        int idx = 1;

        switch (buffer[0]) {
            case 'S':
                SaveLoad::StateFromAscii(buffer, data);
                break;
            case 'M':
                map->LoadAscii(buffer, &idx);
                break;
            case 'E':
                EntityTools::AppendEntity(entities, SaveLoad::NewEntityFromAscii(buffer), maxEntities);
                break;
            case 'P':
                // TODO: Load puzzle data
                break;
        }
    }
    free(buffer);
}


// SAVING //

bool SaveLoad::SaveGameValid() {
    std::string filename = "save/STATE";

    fstream saveFile;
    saveFile.open(filename.c_str());
    if (!saveFile.is_open()) {
        return false;
    }

    char next = saveFile.get();
    if (saveFile.eof() || 'S' != next) {
        return false;
    }
    saveFile.close();

    return true;
}
bool SaveLoad::Save(GameData* data) {
    // TODO: Check if save folder exists
    // TODO: Multiple saves?

    bool valid = true;
    
    valid = SaveState(data);
    for (int x = 0; x < WorldWidth && valid; x++) {
        for (int y = 0; y < WorldHeight && valid; y++) {
            valid = SaveMap(data->world[x][y], x, y);
        }
    }

    // TODO: Pack saves into one file?

    return valid;
}
bool SaveLoad::SaveState(GameData* data) {
    string filename = "save/STATE";
    ofstream saveFile;
    saveFile.open(filename.c_str(), ios::out | ios::trunc);
    if (!saveFile.is_open()) {
        perror("Failed to save STATE file:\n");
        printf("\t%s\n", filename.c_str());
        return false;
    }

    string* text = new string();
    strutil::appendChar(text, 'S'); // Identify first line as a state
    strutil::poolIntegers(text, 3, data->ticks, data->worldX, data->worldY); // Save number of ticks and currently loaded map position
    strutil::appendChar(text, '\n');

    for (int i = 0; i < MaxEntities; i++) { // Save and store entity data
        if (data->entities[i]) {
            string* entityData = data->entities[i]->Ascii();
            strutil::appendChar(text, '\n');
            text->append(*entityData);
            delete entityData;
        }
    }

    saveFile << text->c_str();
    saveFile.close();
    delete text;
    
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