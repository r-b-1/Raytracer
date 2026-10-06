#ifndef BLINN_PHONG_SHADER_H
#define BLINN_PHONG_SHADER_H

#include "shader.h"

// Lambertian plus a specular highlight. shininess_ is the Phong exponent:
// larger values give a tighter, smaller highlight.
class BlinnPhongShader : public Shader {
public:
    BlinnPhongShader(const color& albedo = color(0.8, 0.8, 0.8),
                     const color& specular = color(1, 1, 1),
                     float shininess = 32.0f,
                     float specular_strength = 1.0f,
                     const PointLight& light = PointLight())
        : Shader(light),
          albedo_(albedo),
          specular_(specular),
          shininess_(shininess),
          specular_strength_(specular_strength) {}

    color rayColor(const HitStruct& h) const override;

    float shininess() const { return shininess_; }

private:
    color albedo_;
    color specular_;
    float shininess_;
    float specular_strength_;
};

#endif
