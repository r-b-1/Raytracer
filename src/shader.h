#ifndef SHADER_H
#define SHADER_H

#include "color.h"
#include "point_light.h"
#include "vec3.h"

// HitStruct is defined in shape.h, which includes this header. Only a forward
// declaration is needed for a const-reference parameter, and it is what keeps
// the two headers from deadlocking on each other.
class HitStruct;

// Base class for surface shading models. Every Shape owns one and the renderer
// calls rayColor once per hit.
class Shader {
public:
    virtual ~Shader() = default;

    // Returns the outgoing color for the surface described by h.
    virtual color rayColor(const HitStruct& h) const = 0;

    void set_light(const PointLight& light) { light_ = light; }
    const PointLight& light() const { return light_; }

protected:
    explicit Shader(const PointLight& light = PointLight()) : light_(light) {}

private:
    PointLight light_;
};

#endif
