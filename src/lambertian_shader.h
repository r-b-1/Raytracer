#ifndef LAMBERTIAN_SHADER_H
#define LAMBERTIAN_SHADER_H

#include "shader.h"

// Diffuse shading, summed over every light on the material.
class LambertianShader : public Shader {
public:
    explicit LambertianShader(const Color& baseColor = Color(0.8, 0.8, 0.8),
                              const PointLight& light = PointLight())
        : Shader(light), baseColor_(baseColor) {}

    LambertianShader(const Color& baseColor, const std::vector<PointLight>& lights)
        : Shader(lights), baseColor_(baseColor) {}

    Color rayColor(const HitStruct& h, const Scene& scene, int depth) const override;

    const Color& baseColor() const { return baseColor_; }

private:
    Color baseColor_;
};

#endif