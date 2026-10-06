#ifndef LAMBERTIAN_SHADER_H
#define LAMBERTIAN_SHADER_H

#include "shader.h"

// Matte / diffuse surface. Brightness is the cosine of the angle between the
// surface normal and the direction to the light, scaled by baseColor.
class LambertianShader : public Shader {
public:
    explicit LambertianShader(const color& baseColor = color(0.8, 0.8, 0.8),
                              const PointLight& light = PointLight())
        : Shader(light), baseColor_(baseColor) {}

    color rayColor(const HitStruct& h) const override;

    const color& baseColor() const { return baseColor_; }

private:
    color baseColor_;
};

#endif
