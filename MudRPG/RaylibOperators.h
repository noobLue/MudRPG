#pragma once
#ifndef RAYLIB_OP_OVERLOADS_HPP_INCLUDED
#define RAYLIB_OP_OVERLOADS_HPP_INCLUDED
#include "raylib.h"
#include "raymath.h"

/*extern Vector2 operator+(const Vector2& a, const Vector2& b);
extern Vector2 operator+=(Vector2& a, const Vector2& b);
extern Vector2& operator-(Vector2& a);
extern Vector2 operator-(const Vector2& a, const Vector2& b);
extern Vector2& operator-=(Vector2& a, const Vector2& b);
extern Vector2 operator*(const Vector2& a, float b);
extern Vector2& operator*=(Vector2& a, const float b);
extern Vector2 operator/(const Vector2& a, const float b);*/


inline Vector2 operator+(const Vector2& a, const Vector2& b) {
    return Vector2Add(a, b);
}

inline Vector2 operator+=(Vector2& a, const Vector2& b) {
    a = a + b;
    return a;
}

//Negation: Unary Minus operator
inline Vector2& operator-(Vector2& a) {
    a = Vector2Negate(a);
    return a;
}

inline Vector2 operator-(const Vector2& a, const Vector2& b) {
    return Vector2Subtract(a, b);
}

inline Vector2& operator-=(Vector2& a, const Vector2& b) {
    a = a - b;
    return a;
}

//Multiplication overload only provides for multiplying a vector by a scalar.   Vector * Vector is not overloaded to avoid confusion whether one intends a dot product, cross product, etc.
inline Vector2 operator*(const Vector2& a, float b) {
    return Vector2Scale(a, b);
}

inline Vector2& operator*=(Vector2& a, const float b) {
    a = a * b;
    return a;
}

//Division overload: Merely scalar multiplication by the reciprocal, with a Divide-By-Zero check.
inline Vector2 operator/(const Vector2& a, const float b) {
    // assert (b != 0.0);
    float recip = 1.0 / b;
    return a * recip;
}


#endif // RAYLIB-OP-OVERLOADS_HPP_INCLUDED