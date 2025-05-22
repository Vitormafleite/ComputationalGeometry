#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class Vector3{
public:

    float x,y,z;

    Vector3();
    Vector3(float x, float y, float z);

    // Basic Operations
    // Here we assign/overload the basic operators +-*/ for the Vector3 class, returning a new Vector3
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    // Compound assignment
    // Here we do the same, however with compound assignments, returning the original object with the new value
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Vector operations 
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float length() const;
    Vector3 normalized() const;

};

#endif //VECTOR3_H
