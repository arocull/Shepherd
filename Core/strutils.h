#pragma once

#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdarg.h>

// Appends a single character to a C string
void strAppendChar(char* string, char toAdd);
// Appends a single character to a C++ string
void strAppendChar(std::string* string, char toAdd);
// Appends X number of integers from a parameter to list to a C++ string
void strPoolIntegers(std::string* string, int totalNums, ...);