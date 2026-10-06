#include "blinn_phong_shader.h"

#include <algorithm>
#include <cmath>

#include "shape.h"

color BlinnPhongShader::rayColor(const HitStruct& h) const {
    vec3 lightDir = unit_vector(light().position() - h.p());

    // The view direction points from the surface back toward the camera, so it
    // is the reverse of the ray that produced this hit.
    vec3 viewDir = unit_vector(-h.ray_direction());

    // Blinn's trick: the halfway vector between light and view, so the exponent
    // tightens the highlight without needing a reflection vector.
    vec3 halfVector = unit_vector(lightDir + viewDir);

    float nDotl = std::max(0.0f, dot(h.normal(), lightDir));

    // No direct light means no highlight either. Without this early return a
    // grazing half-vector can light the specular term on a surface the diffuse
    // term has already blacked out, which reads as a floating bright spot.
    if (nDotl <= 0.0f)
        return color(0, 0, 0);

    float nDoth = std::max(0.0f, dot(h.normal(), halfVector));
    float highlight = std::pow(nDoth, shininess_) * specular_strength_;

    return baseColor_ * nDotl + specular_ * highlight;
}
