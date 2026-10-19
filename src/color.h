#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

using Color = Vec3; // here we are using the matrix math from the vec3.h file to create a color class to color objects.


inline Color sky_color(const Vec3& direction) {
    float a = 0.5f * (unit_vector(direction).y() + 1.0f);
    return (1.0f - a) * Color(1.0f, 1.0f, 1.0f) + a * Color(0.5f, 0.7f, 1.0f);
}

#endif
