#ifndef NORMAL_SHADER_H
#define NORMAL_SHADER_H

#include "shader.h"

// Visualizes surface normals as RGB colors.
class NormalShader : public Shader {
public:
    explicit NormalShader(const PointLight& light = PointLight()) : Shader(light) {}

    color rayColor(const HitStruct& h) const override;
};

#endif
