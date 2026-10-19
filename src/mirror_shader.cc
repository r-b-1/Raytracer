#include "mirror_shader.h"

#include "scene.h"

Color MirrorShader::rayColor(const HitStruct& h, const Scene& scene, int depth) const {

    Vec3 reflected = unit_vector(reflect(h.ray_direction(), h.normal()));

    return baseColor_ * trace(scene, Ray(h.p(), reflected), depth - 1);
}
