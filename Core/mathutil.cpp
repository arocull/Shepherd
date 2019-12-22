// Math Functions
// All C code, marked as .cpp for easy compilation purposes

#include "Core/mathutil.h"


// Returns the Pythagorean distance between two points--tailored for floats specifically
float distF(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}


// Returns a random float between 0 and 1
float RandomF() {
    return (float) rand() / (float) RAND_MAX;
}
// Returns a random integer between the given range, inclusive
int RandomI(int a, int b) {
    return rand() % (b-a) + a;
}