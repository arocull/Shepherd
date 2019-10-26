#pragma once

// Math Functions


/*
sgn(n) - Gets and returns -1, 0, or 1 based off of the sign of the given value

- Taken from https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
*/
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}