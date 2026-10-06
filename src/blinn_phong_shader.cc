#include "blinn_phong_shader.h"

#include <algorithm>
#include <cmath>

#include "shape.h"

color BlinnPhongShader::rayColor(const HitStruct& h) const {
    vec3 lightDir = unit_vector(light().position() - h.p());

    // Reverse the incoming ray to point back toward the viewer.
    vec3 viewDir = unit_vector(-h.ray_direction());

    vec3 halfVector = unit_vector(lightDir + viewDir);

    float nDotl = std::max(0.0f, dot(h.normal(), lightDir));

    // Suppress highlights on the unlit side.
    if (nDotl <= 0.0f)
        return color(0, 0, 0);

    float nDoth = std::max(0.0f, dot(h.normal(), halfVector));
    float highlight = std::pow(nDoth, shininess_) * specular_strength_;

    return baseColor_ * nDotl + specular_ * highlight;
}
