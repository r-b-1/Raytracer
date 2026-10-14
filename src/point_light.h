#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "color.h"
#include "vec3.h"

// Point light: a position plus how brightly it radiates in each channel.
class PointLight {
public:
    PointLight() = default;
    explicit PointLight(const point3& position) : position_(position) {}
    PointLight(const point3& position, const color& intensity)
        : position_(position), intensity_(intensity) {}

    const point3& position() const { return position_; }
    void set_position(const point3& position) { position_ = position; }

    // Radiance multiplier; white by default.
    const color& intensity() const { return intensity_; }
    void set_intensity(const color& intensity) { intensity_ = intensity; }

private:
    point3 position_ = point3(0, 10, 5);
    color intensity_ = color(1, 1, 1);
};

#endif