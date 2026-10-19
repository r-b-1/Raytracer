#include "mirror_shader.h"

#include "scene.h"

Color MirrorShader::rayColor(const HitStruct& h, const Scene& scene, int depth) const {
    // Reflect the incoming ray about the surface normal. The camera stores
    // that direction on the hit, the same way the other shaders read it.
    Vec3 reflected = unit_vector(reflect(h.ray_direction(), h.normal()));

    // Trace the reflected ray through the scene, so the surface shows the
    // other objects and not just the sky. Spending a bounce keeps recursive
    // reflections (two mirrors facing each other) terminating.
    return baseColor_ * trace(scene, Ray(h.p(), reflected), depth - 1);
}
