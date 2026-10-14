#include "lambertian_shader.h"

#include <algorithm>

#include "shape.h"

color LambertianShader::rayColor(const HitStruct& h, const Scene&, int) const {
    color out(0, 0, 0);

    for (const auto& light : lights()) {
        vec3 lightDir = unit_vector(light.position() - h.p());

        // Surfaces facing away from a light receive none of its direct light.
        float nDotl = std::max(0.0f, dot(h.normal(), lightDir));

        out += light.intensity() * (baseColor_ * nDotl);
    }

    return out;
}