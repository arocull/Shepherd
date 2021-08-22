#include "Core/strutils.h"

void strAppendChar(char* string, char toAdd) {
    strncat(string, &toAdd, 1);
}
void strAppendChar(std::string string, char toAdd) {
    string.append(&toAdd, 1);
}