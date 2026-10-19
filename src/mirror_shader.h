#ifndef MIRROR_SHADER_H
#define MIRROR_SHADER_H

#include "shader.h"

// Reflects the background, tinted by the mirror color.
class MirrorShader : public Shader {
public:
    explicit MirrorShader(const Color& baseColor = Color(0.8, 0.8, 0.8),
                          const PointLight& light = PointLight())
        : Shader(light), baseColor_(baseColor) {}

    // A mirror reflects rather than shades, so the lights are carried only so
    // the material can be constructed like every other one.
    MirrorShader(const Color& baseColor, const std::vector<PointLight>& lights)
        : Shader(lights), baseColor_(baseColor) {}

    Color rayColor(const HitStruct& h, const Scene& scene, int depth) const override;

    const Color& baseColor() const { return baseColor_; }

private:
    Color baseColor_;
};

#endif