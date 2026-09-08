#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
    public:
        point3 p;
        vec3 normal;
        float t;
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const = 0;

};

#endif