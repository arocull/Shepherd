#pragma once

#include <math.h>

// Math Functions

/*
sgn(n) - Gets and returns -1, 0, or 1 based off of the sign of the given value

- Taken from https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
*/
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// Returns the minimum of two values--works for any numerical type
template <typename T> T min(T val1, T val2) {
    if (val1 > val2)
        return val2;
    else
        return val1;
}
// Returns the maximum of two values--works for any numerical type
template <typename T> T max(T val1, T val2) {
    if (val1 > val2)
        return val1;
    else
        return val2;
}


// Returns the Pythagorean distance between two points
template <typename T> int dist(T x1, T y1, T x2, T y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}


// Lops off the left side of the string up until X point
// Used guide at http://www.martinbroadhurst.com/trim-a-string-in-c.html
/*char *string_leftTrim(char *str, int trimTo) {
    size_t trimSize = sizeof(char)*trimTo;
    
    int i = 0;//strlen(str);
    while (i < trimTo) {
        str[i] = '\0';  //Set character to terminating character
        i++;
    }

    // Move terminating characters to end of string
    //memmove(str, str + trimSize, )

    return str;
}*/