#pragma once

#include <cmath>

#define PI 3.14158265

// Math Functions

template <typename T> int sgn(T val);

template <typename T> T min(T val1, T val2);
template <typename T> T max(T val1, T val2);

template <typename T> int dist(T x1, T y1, T x2, T y2);

template <typename T> T distGrid(T x1, T y1, T x2, T y2);

template <typename T> T lerp(T a, T b, float alpha);

float distF(float x1, float y1, float x2, float y2);

float RandomF();
int RandomI(int a, int b);





/*
sgn(n) - Gets and returns -1, 0, or 1 based off of the sign of the given value

- Taken from https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
*/
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// Returns the minimum of two values--works for any numerical type
template <typename T> T min(T val1, T val2) {
    return val1 > val2 ? val2 : val1;
}
// Returns the maximum of two values--works for any numerical type
template <typename T> T max(T val1, T val2) {
    return val1 > val2 ? val1 : val2;
}


// Returns the Pythagorean distance between two points--works with any numerical type
template <typename T> int dist(T x1, T y1, T x2, T y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}

// Returns the gridded distance between two points
template <typename T> T distGrid(T x1, T y1, T x2, T y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

// Returns a linearly interpolated value between values A and B using alpha
template <typename T> T lerp(T a, T b, float alpha) {
    return a * (1.0f - alpha) + b * alpha;
}