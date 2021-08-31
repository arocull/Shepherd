#include "Core/strutils.h"

void strutil::appendChar(char* string, char toAdd) {
    strncat(string, &toAdd, 1);
}
void strutil::appendChar(std::string* string, char toAdd) {
    string->append(&toAdd, 1);
}

void strutil::poolIntegers(std::string* string, int totalNums, ...) {
    va_list ap; // Grab the argument list

    va_start(ap, totalNums); // Start processing the list
    for (int i = 0; i < totalNums; i++) {
        int item = va_arg(ap, int); // Grabs the next arguement in the list as an integer, and iterates forward
        
        // Converts the number to a string, adds it to the list, and puts a comma at the end (if not the last one)
        std::string itemText = to_string(item);
        string->append(itemText);
        if (!(i == totalNums - 1)) {
            appendChar(string, ',');
        }
    }
};

int strutil::parseInt(char* string, int* index) {
    int i = *index; // Load index value
    int len = 0;

    int num = 0;
    bool negative = false;

    // Check to see if the number is negative
    if (string[i] == '-') {
        negative = true;
        i++;
    }
    // Get number of digits
    for (int z = i; isCharNumeric(string[z]); z++) {
        printf("Observing %c\n", string[z]);
        len++;
    }

    // For each digit, convert the digit to a number, and then multiply it by the 10 to the power of the digit's place
    for (int z = i; len > 0; len--, z++) {
        num += ((int) (string[z] - '0')) * ((int) pow(10, len - 1));
        i++;
    }

    if (negative) {
        num *= -1;
    }

    *index = i+1; // Update index value, move an extra char so we are past the comma or ender
    return num;
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