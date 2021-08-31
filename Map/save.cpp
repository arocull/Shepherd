#include "Map/save.h"

bool SaveLoad::SaveMap(Map* map, int x, int y) {
    string* data = map->Ascii();

    string filename = "save/m"; // Indicate this is a map
    filename.append(std::to_string(x)); // X position
    strAppendChar(&filename, ','); // Comma
    filename.append(std::to_string(y)); // Y position

    strAppendChar(data, '\n');

    // Push in Entity Data
    for (int i = 0; i < MaxEntitiesStoreable; i++) {
        if (nullptr != map->StoredEntities[i]) {
            Entity* obj = map->StoredEntities[i];
            std::string* entityData = obj->Ascii();

            strAppendChar(data, '\n');
            data->append(*entityData);
            delete entityData;
        }
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