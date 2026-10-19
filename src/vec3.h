#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <cstdlib>
#include <iostream>

// Random real in [0, 1). RAND_MAX is not exactly representable as a float,
// so the fraction is formed in double and then stored as a float.
inline float random_float() {
    return static_cast<float>(std::rand() / (RAND_MAX + 1.0));
}

// Random real in [min, max).
inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

class Vec3 {
    public:
    float e[3];

    Vec3() : e{0,0,0} {}
    Vec3(float e0, float e1, float e2) : e{e0,e1,e2} {}
    float x() const { return e[0];}
    float y() const { return e[1];}
    float z() const { return e[2];}

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    float operator[](int i) const { return e[i]; }
    float& operator[](int i) {return e[i]; }

    Vec3& operator+=(const Vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3& operator*=(float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(float t) {
            return *this *= 1/t;
    }

    float length() const {
        return std::sqrt(length_squared()); //This is doing something that we did in class. Like normalizing or something its ||v||.
    }
    
    float length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; // This is where we are "Squaring" the length of the vector by just multiplying the vector by itself.
    }

    static Vec3 random() {
        return Vec3(random_float(), random_float(), random_float());
    }
    static Vec3 random(float min, float max) {
        return Vec3(random_float(min,max), random_float(min,max), random_float(min,max));
    }
};

// Here we are setting Point3 as an alias for Vec3, it makes it easier for geometric clarity in the code later.
using Point3 = Vec3;

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]); // This is used to add two vectors together.
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]); // This is used to subtract two vectors from each other.
}

inline Vec3 operator*(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]); // This is used to multiply two vectors together.
}

inline Vec3 operator*(float t, const Vec3& v) {
    return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]); //Not 100% sure on this one.
}

inline Vec3 operator*(const Vec3& v, float t) {
    return t * v; //Not 100% sure on this one.
}

inline Vec3 operator/(const Vec3& v, float t) {
    return (1/t) * v; //Not 100% sure on this one.
}

inline float dot(const Vec3& u, const Vec3& v) { // This one is used to calculate the dot product of two vectors.
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
    u.e[2] * v.e[0] - u.e[0] * v.e[2],
    u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(const Vec3& v) {
    return v / v.length(); // This i used to calculate the unit vector of a vector.
}

// Reflect v about a unit normal n.
inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*dot(v, n)*n;
}

inline Vec3 random_unit_vector() {
    while (true) {
        auto p = Vec3::random(-1,1);
        auto lensq = p.length_squared();
        // 1e-160 underflows to 0 in float, so reject only lengths too small to invert.
        if (1e-16f < lensq && lensq <= 1.0f)
            return p / std::sqrt(lensq);
    }
}

inline Vec3 random_on_hemisphere(const Vec3& normal) {
    Vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0f) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

#endif