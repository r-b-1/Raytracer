#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"
#include "rtweekend.h"

using color = vec3; // here we are using the matrix math from the vec3.h file to create a color class to color objects.

// The vertical sky gradient shown wherever a ray misses every shape. Shared by
// the camera and by materials that trace secondary rays, so a reflection can
// never disagree with the backdrop. `direction` does not have to be normalized.
inline color sky_color(const vec3& direction) {
    float a = 0.5f * (unit_vector(direction).y() + 1.0f);
    return (1.0f - a) * color(1.0f, 1.0f, 1.0f) + a * color(0.5f, 0.7f, 1.0f);
}

inline void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    //Translate the [0,1] component values to the color byte ranges [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(255.999 * intensity.clamp(r));
    int gbyte = int(255.999 * intensity.clamp(g));
    int bbyte = int(255.999 * intensity.clamp(b));

    //Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif
