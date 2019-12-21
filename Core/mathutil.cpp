// Math Functions
// All C code, marked as .cpp for easy compilation purposes

#include "Core/mathutil.h"



// Returns a random float between 0 and 1
float RandomF() {
    return (float) rand() / (float) RAND_MAX;
}
// Returns a random integer between the given range, inclusive
int RandomI(int a, int b) {
    return rand() % (b-a) + a;
}