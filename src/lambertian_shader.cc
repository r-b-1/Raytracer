#include "lambertian_shader.h"

#include <algorithm>

#include "scene.h"
#include "shape.h"

Color LambertianShader::rayColor(const HitStruct& h, const Scene& scene, int) const {
    Color out(0, 0, 0);

    for (const auto& light : lights()) {
        Vec3 lightDir = unit_vector(light->position() - h.p());

        // Surfaces facing away from a light receive none of its direct light.
        float nDotl = std::max(0.0f, dot(h.normal(), lightDir));
        if (nDotl <= 0.0f)
            continue;

        // Reject on the cheap dot product before paying for a shadow ray.
        if (occluded(scene, h.p(), h.normal(), light->position()))
            continue;

        out += light->intensity() * (baseColor_ * nDotl);
    }

    return out;
}
