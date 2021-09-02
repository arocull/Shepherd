#pragma once

#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdarg.h>

// String Util - Utility functions for rapid and easy string manipulation, formation, and parsing
namespace strutil {
    using std::string;
    using std::to_string;

    // Appends a single character to a C string
    void appendChar(char* string, char toAdd);
    // Appends a single character to a C++ string
    void appendChar(std::string* string, char toAdd);

    // Appends X number of integers from a parameter to list to a C++ string
    void poolIntegers(std::string* string, int totalNums, ...);

    // Given a string, searches for the next integer and begins parsing it until a non-number is present
    // - Takes in an integer pointer for the starting index, and iterates it until it hits the end of the current integer
    int parseInt(char* string, int* index);
    // Given a string, returns true or false?
    // - Takes in an integer pointer for the starting index, and iterates it until it hits the end of the current integer
    bool parseBool(char* string, int* index);

    // Returns true if the given character is a number 0-9
    bool isCharNumeric(char val);
}