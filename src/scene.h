#pragma once

#include <limits>
#include <memory>
#include <vector>

#include "point_light.h"
#include "shape.h"

class Scene {
public:
    std::vector<std::shared_ptr<Shape>> objects;

    PointLight light;

    bool intersect(const ray& r, double tmin, double& tmax, HitStruct& hit) const {
        bool found = false;

        for (const auto& object : objects) {
            HitStruct candidate;

            if (object->intersect(r, tmin, tmax, candidate)) {
                tmax = candidate.t();
                hit = candidate;
                found = true;
            }
        }

        return found;
    }
};

// Trace one ray and return the color seen along it: the shading of the first
// surface it hits, or the sky if it escapes. `depth` is the remaining
// reflection budget, so recursive bounces terminate. Shared by the camera and
// by materials that trace secondary rays, so both resolve hits the same way.
inline color trace(const Scene& scene, const ray& r, int depth) {
    if (depth <= 0)
        return color(0, 0, 0); // Out of bounces; absorbing rather than recursing.

    double tmax = std::numeric_limits<double>::infinity();
    HitStruct rec;

    if (scene.intersect(r, k_min_hit_t, tmax, rec)) {
        if (rec.shader())
            return rec.shader()->rayColor(rec, scene, depth);
        return 0.5f * (rec.normal() + color(1, 1, 1)); // Fallback for an unshaded hit.
    }

    return sky_color(r.direction());
}

// True when something blocks the line from `p` to `light_pos`.
inline bool occluded(const Scene& scene, const point3& p, const vec3& normal,
                     const point3& light_pos) {
    const vec3 to_light = light_pos - p;

    // Step off the surface before testing. Without this the shadow ray starts
    // exactly on the surface it is testing and hits it immediately, so
    // everything would read as shadowed.
    const point3 origin = p + k_shadow_eps * normal;

    // Shape::intersect reports t in units of the direction vector, so the
    // direction must be normalized before tmax can be a distance. Left
    // unnormalized, tmax would be squared by |direction| and the ray would
    // reach far past the light.
    double tmax = to_light.length(); // A hit *beyond* the light does not block.
    HitStruct rec;
    return scene.intersect(ray(origin, unit_vector(to_light)), k_min_hit_t, tmax, rec);
}
