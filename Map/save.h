#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <filesystem>

#include "Core/strutils.h"
#include "Map/map.h"
#include "Entities/entity.h"

// FStream stuff referenced from https://www.cplusplus.com/doc/tutorial/files/

namespace SaveLoad {
    using std::ios;
    using std::ofstream;
    using std::fstream;
    using std::string;
    //using std::filesystem;

    bool SaveMap(Map* map, int x, int y);
};