#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "color.h"
#include "vec3.h"

// White point light.
class PointLight {
public:
    PointLight() = default;
    explicit PointLight(const point3& position) : position_(position) {}

    const point3& position() const { return position_; }
    void set_position(const point3& position) { position_ = position; }

private:
    point3 position_ = point3(0, 10, 5);
};

#endif
