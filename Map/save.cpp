#include "Map/save.h"

bool SaveLoad::SaveMap(Map* map, int x, int y) {
    string* data = map->Ascii();

    string filename = "save/m"; // Indicate this is a map
    filename.append(std::to_string(x)); // X position
    strAppendChar(&filename, ','); // Comma
    filename.append(std::to_string(y)); // Y position

    // printf("Map data\n%s\n\tfor filename %s\n", data->c_str(), filename.c_str());

    // TODO: Entity data

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