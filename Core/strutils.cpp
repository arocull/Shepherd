#include "Core/strutils.h"

void strAppendChar(char* string, char toAdd) {
    strncat(string, &toAdd, 1);
}
void strAppendChar(std::string* string, char toAdd) {
    string->append(&toAdd, 1);
}

void strPoolIntegers(std::string* string, int totalNums, ...) {
    va_list ap; // Grab the argument list

    va_start(ap, totalNums); // Start processing the list
    for (int i = 0; i < totalNums; i++) {
        int item = va_arg(ap, int); // Grabs the next arguement in the list as an integer, and iterates forward
        
        // Converts the number to a string, adds it to the list, and puts a comma at the end (if not the last one)
        std::string itemText = std::to_string(item);
        string->append(itemText);
        if (!(i == totalNums - 1)) {
            strAppendChar(string, ',');
        }
    }
};