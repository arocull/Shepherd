#pragma once

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