#ifndef LAMBERTIAN_SHADER_H
#define LAMBERTIAN_SHADER_H

#include "shader.h"

// Matte / diffuse surface. Brightness is the cosine of the angle between the
// surface normal and the direction to the light, scaled by the albedo.
class LambertianShader : public Shader {
public:
    explicit LambertianShader(const color& albedo = color(0.8, 0.8, 0.8),
                              const PointLight& light = PointLight())
        : Shader(light), albedo_(albedo) {}

    color rayColor(const HitStruct& h) const override;

    const color& albedo() const { return albedo_; }

private:
    color albedo_;
};

#endif
