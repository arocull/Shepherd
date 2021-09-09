#include "Core/strutils.h"

void strutil::appendChar(char* str, char toAdd) {
    strncat(str, &toAdd, 1);
}
void strutil::appendChar(std::string* str, char toAdd) {
    str->append(&toAdd, 1);
}

void strutil::poolIntegers(std::string* str, int totalNums, ...) {
    va_list ap; // Grab the argument list

    va_start(ap, totalNums); // Start processing the list
    for (int i = 0; i < totalNums; i++) {
        int item = va_arg(ap, int); // Grabs the next arguement in the list as an integer, and iterates forward
        
        // Converts the number to a string, adds it to the list, and puts a comma at the end (if not the last one)
        std::string itemText = to_string(item);
        str->append(itemText);
        if (!(i == totalNums - 1)) {
            appendChar(str, ',');
        }
    }
};

int strutil::parseInt(char* str, int* index) {
    int i = *index; // Load index value
    int len = 0;

    int num = 0;
    bool negative = false;

    // Check to see if the number is negative
    if (str[i] == '-') {
        negative = true;
        i++;
    } else if (!isCharNumeric(str[i])) { // Otherwise, if this is not numeric, throw an error
        throw str[i];
    }

    // Get number of digits
    for (int z = i; isCharNumeric(str[z]); z++) {
        len++;
    }

    // For each digit, convert the digit to a number, and then multiply it by the 10 to the power of the digit's place
    for (int z = i; len > 0; len--, z++) {
        num += ((int) (str[z] - '0')) * ((int) pow(10, len - 1));
        i++;
    }

    if (negative) {
        num *= -1;
    }

    *index = i+1; // Update index value, move an extra char so we are past the comma or ender
    return num;
}
bool strutil::parseBool(char* str, int* index) {
    return (bool) parseInt(str, index); // Parse int, but cast to boolean. TODO: Should we just grab the next character instead?
}

bool strutil::isCharNumeric(char val) {
    switch (val) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
    }

    return false;
}