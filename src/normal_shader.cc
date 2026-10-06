#include "normal_shader.h"

#include "shape.h"

color NormalShader::rayColor(const HitStruct& h) const {
    // This is the shading hack that used to live inline in camera::ray_color.
    // A normal of (-1,-1,-1) lands on black, (+1,+1,+1) on white.
    return 0.5f * (h.normal() + color(1, 1, 1));
}
