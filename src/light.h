#pragma once

#include "color.h"
#include "vec3.h"

class Light {
public:
    virtual ~Light() = default;
    virtual const Point3& position() const = 0;
    virtual const Color& intensity() const = 0;
};
