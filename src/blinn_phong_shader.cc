#include "blinn_phong_shader.h"

#include <algorithm>
#include <cmath>

#include "scene.h"
#include "shape.h"

color BlinnPhongShader::rayColor(const HitStruct& h, const Scene& scene, int) const {
    color out(0, 0, 0);

    // Reverse the incoming ray to point back toward the viewer.
    vec3 viewDir = unit_vector(-h.ray_direction());

    for (const auto& light : lights()) {
        vec3 lightDir = unit_vector(light.position() - h.p());

        // Suppress the highlight on the unlit side.
        float nDotl = dot(h.normal(), lightDir);
        if (nDotl <= 0.0f)
            continue;

        // Reject on the cheap dot product before paying for a shadow ray.
        if (occluded(scene, h.p(), h.normal(), light.position()))
            continue;

        vec3 halfVector = unit_vector(lightDir + viewDir);

        float nDoth = std::max(0.0f, dot(h.normal(), halfVector));
        float highlight = std::pow(nDoth, shininess_) * specular_strength_;

        out += light.intensity() * (baseColor_ * nDotl + specular_ * highlight);
    }

    return out;
}