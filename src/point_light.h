#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "color.h"
#include "vec3.h"

// Point light: a position plus how brightly it radiates in each channel.
class PointLight {
public:
    PointLight() = default;
    explicit PointLight(const Point3& position) : position_(position) {}
    PointLight(const Point3& position, const Color& intensity)
        : position_(position), intensity_(intensity) {}

    const Point3& position() const { return position_; }
    void set_position(const Point3& position) { position_ = position; }

    // Radiance multiplier; white by default.
    const Color& intensity() const { return intensity_; }
    void set_intensity(const Color& intensity) { intensity_ = intensity; }

private:
    Point3 position_ = Point3(0, 10, 5);
    Color intensity_ = Color(1, 1, 1);
};

#endif