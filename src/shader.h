#ifndef SHADER_H
#define SHADER_H

#include "color.h"
#include "point_light.h"
#include "vec3.h"

// Defined in shape.h; forward-declared to avoid a circular include.
class HitStruct;

class Shader {
public:
    virtual ~Shader() = default;

    virtual color rayColor(const HitStruct& h) const = 0;

    void set_light(const PointLight& light) { light_ = light; }
    const PointLight& light() const { return light_; }

protected:
    explicit Shader(const PointLight& light = PointLight()) : light_(light) {}

private:
    PointLight light_;
};

#endif
