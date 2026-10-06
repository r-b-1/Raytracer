#ifndef NORMAL_SHADER_H
#define NORMAL_SHADER_H

#include "shader.h"

// Maps the surface normal straight to RGB. Not a lighting model - it ignores
// the light entirely - which makes it a sensible default for any shape that
// has not been given a real material yet.
class NormalShader : public Shader {
public:
    explicit NormalShader(const PointLight& light = PointLight()) : Shader(light) {}

    color rayColor(const HitStruct& h) const override;
};

#endif
