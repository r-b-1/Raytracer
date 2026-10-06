#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "color.h"
#include "vec3.h"

// A light at a single point in the scene. The lab assumes a white light, so
// only the position is modelled; add a color field if you ever need a tint.
class PointLight {
public:
    PointLight() = default;
    explicit PointLight(const point3& position) : position_(position) {}

    const point3& position() const { return position_; }
    void set_position(const point3& position) { position_ = position; }

private:
    // The lab's suggested default: above and slightly behind the camera.
    point3 position_ = point3(0, 10, 5);
};

#endif
