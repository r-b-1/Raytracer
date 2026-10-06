#ifndef BLINN_PHONG_SHADER_H
#define BLINN_PHONG_SHADER_H

#include "shader.h"

// Diffuse shading with a specular highlight.
class BlinnPhongShader : public Shader {
public:
    BlinnPhongShader(const color& baseColor = color(0.8, 0.8, 0.8),
                     const color& specular = color(1, 1, 1),
                     float shininess = 32.0f,
                     float specular_strength = 1.0f,
                     const PointLight& light = PointLight())
        : Shader(light),
          baseColor_(baseColor),
          specular_(specular),
          shininess_(shininess),
          specular_strength_(specular_strength) {}

    color rayColor(const HitStruct& h) const override;

    float shininess() const { return shininess_; }

private:
    color baseColor_;
    color specular_;
    float shininess_; // Higher exponents produce smaller highlights.
    float specular_strength_;
};

#endif
