#ifndef LAMBERTIAN_SHADER_H
#define LAMBERTIAN_SHADER_H

#include "shader.h"

// Diffuse shading, summed over every light on the material.
class LambertianShader : public Shader {
public:
    explicit LambertianShader(const color& baseColor = color(0.8, 0.8, 0.8),
                              const PointLight& light = PointLight())
        : Shader(light), baseColor_(baseColor) {}

    LambertianShader(const color& baseColor, const std::vector<PointLight>& lights)
        : Shader(lights), baseColor_(baseColor) {}

    color rayColor(const HitStruct& h, const Scene& scene, int depth) const override;

    const color& baseColor() const { return baseColor_; }

private:
    color baseColor_;
};

#endif