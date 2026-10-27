#ifndef SCENE_H
#define SCENE_H

#include <limits>
#include <memory>
#include <vector>

#include "point_light.h"
#include "shape.h"

class Scene {
public:
    std::vector<std::shared_ptr<Shape>> objects;

    PointLight light;

    bool intersect(const Ray& r, float tmin, float& tmax, HitStruct& hit) const {
        bool found = false;

        for (const auto& object : objects) {
	    if (!object->bounding_box().hit(r, interval(tmin, tmax))) // Uses aabb to check if the bb is hit, if not there is no object hit
		continue;


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


inline Color trace(const Scene& scene, const Ray& r, int depth) {
    if (depth <= 0)
        return Color(0, 0, 0); // Out of bounces, absorbing rather than recursing.

    float tmax = std::numeric_limits<float>::infinity();
    HitStruct rec;

    if (scene.intersect(r, k_min_hit_t, tmax, rec)) {
        if (rec.shader())
            return rec.shader()->rayColor(rec, scene, depth);
        return 0.5f * (rec.normal() + Color(1, 1, 1)); // Fallback for an unshaded hit.
    }

    return sky_color(r.direction());
}

// True when something blocks the line from p to light_pos.
inline bool occluded(const Scene& scene, const Point3& p, const Vec3& normal,
                     const Point3& light_pos) {
    const Vec3 to_light = light_pos - p;

    // Step off the surface before testing. Without this the shadow ray starts
    // exactly on the surface it is testing and hits it immediately, so
    // everything would read as shadowed.
    const Point3 origin = p + k_shadow_eps * normal;

    // Shape::intersect reports t in units of the direction vector, so the
    // direction must be normalized before tmax can be a distance. Left
    // unnormalized, tmax would be squared by |direction| and the ray would
    // reach far past the light.
    float tmax = to_light.length(); 
    HitStruct rec;
    return scene.intersect(Ray(origin, unit_vector(to_light)), k_min_hit_t, tmax, rec);
}

#endif
