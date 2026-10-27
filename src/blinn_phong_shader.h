#ifndef BLINN_PHONG_SHADER_H
#define BLINN_PHONG_SHADER_H

#include "shader.h"

// Diffuse shading with a specular highlight, summed over every light on the
// material.
class BlinnPhongShader : public Shader {
public:
    BlinnPhongShader(const Color& baseColor = Color(0.8, 0.8, 0.8),
                     const Color& specular = Color(1, 1, 1),
                     float shininess = 32.0f,
                     float specular_strength = 1.0f,
                     const PointLight& light = PointLight())
        : Shader(light),
          baseColor_(baseColor),
          specular_(specular),
          shininess_(shininess),
          specular_strength_(specular_strength) {}

    BlinnPhongShader(const Color& baseColor,
                     const std::vector<std::shared_ptr<Light>>& lights,
                     const Color& specular = Color(1, 1, 1),
                     float shininess = 32.0f,
                     float specular_strength = 1.0f)
        : Shader(lights),
          baseColor_(baseColor),
          specular_(specular),
          shininess_(shininess),
          specular_strength_(specular_strength) {}

    Color rayColor(const HitStruct& h, const Scene& scene, int depth) const override;

    float shininess() const { return shininess_; }

private:
    Color baseColor_;
    Color specular_;
    float shininess_; // Higher exponents produce smaller highlights.
    float specular_strength_;
};

#endif
