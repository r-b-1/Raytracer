#include "lambertian_shader.h"

#include <algorithm>
#include <cmath>

#include "shape.h"

color LambertianShader::rayColor(const HitStruct& h) const {
    // Unit vector from the surface point toward the light. Must be normalized,
    // otherwise the dot product below is not a cosine.
    vec3 lightDir = unit_vector(light().position() - h.p());

    // nDotl is the cosine of the angle between the surface and the light.
    // Clamping at zero is the one gotcha from the slides: a negative value
    // means the surface faces away from the light, and letting it through
    // produces negative radiance that the framebuffer clamps to hard black.
    float nDotl = std::max(0.0f, dot(h.normal(), lightDir));

    return albedo_ * nDotl;
}
