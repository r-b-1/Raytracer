#include "normal_shader.h"

#include "shape.h"

color NormalShader::rayColor(const HitStruct& h) const {
    // Map each normal component from [-1, 1] to [0, 1].
    return 0.5f * (h.normal() + color(1, 1, 1));
}
