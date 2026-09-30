

#pragma once

#include "ray.h"

class HitStruct {
    public:
    point3 p() const {return p_; }
    vec3 normal() const {return front_face_ ? outward_normal_ : -outward_normal_; }
    double t() const { return t_; }
    bool front_face() const {return front_face_; }

    void set_p(const point3& p) { p_ = p; }
    void set_t(double t) { t_ = t; }

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // NOTE: `outward_normal` is assumed to have unit length.
        front_face_ = dot(r.direction(), outward_normal) < 0;
        outward_normal_ = outward_normal;
    }
    
    private:
    point3 p_ = point3(0, 0, 0);
    double t_ = 0;
    bool   front_face_ = true;
    vec3   outward_normal_ = vec3(0, 0, 0);    
};

class Shape {
public:
    virtual ~Shape() = default;

    // Fills `hit` with the nearest intersection in the accepted t range and returns
    // true, or returns false and leaves `hit` untouched.
    //
    // `tmax` is the closest ray parameter found so far. Shapes read this bound;
    // Scene tightens it after a hit. The reference matches the course interface.
    // Every shape accepts an ordinary HitStruct; no derived records are needed.
    virtual bool intersect(
        const ray& r,
        double tmin,
        double& tmax,
        HitStruct& hit
    ) const = 0;
};