#include "lambertian_shader.h"

#include <algorithm>

#include "shape.h"

color LambertianShader::rayColor(const HitStruct& h) const {
    vec3 lightDir = unit_vector(light().position() - h.p());

    // Surfaces facing away from the light receive no direct light.
    float nDotl = std::max(0.0f, dot(h.normal(), lightDir));

    return baseColor_ * nDotl;
}
