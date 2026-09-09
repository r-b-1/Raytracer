#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

#include "rtweekend.h"


class sphere: public hittable {
    public:
        sphere(const point3& center, float radius, const color& sphere_color = color(1,1,1)) : center(center), radius(std::fmax(0, radius)), sphere_color(sphere_color) {}

        bool hit(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const override {
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;

            auto sqrtd = std::sqrt(discriminant);
            
            // Find the nearest root that lise in the acceptable range
            auto root = (h - sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root) {
                root = (h + sqrtd) / a;
                if (root <= ray_tmin || ray_tmax <= root)
                    return false;
            }
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            
            rec.surface_color = sphere_color;
            return true;
        }
    private:
        point3 center;
        float radius;
        color sphere_color;

};

#endif