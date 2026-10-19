#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>

#include "shape.h"
#include "vec3.h"

class Sphere : public Shape {
public:
    Sphere(const Point3& center, float radius,
           std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : Shape(std::move(shader)), center(center), radius(std::fmax(0.0f, radius)) {}

    bool intersect(const Ray& r, float tmin, float& tmax, HitStruct& hit) const override {
        Vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in [tmin, tmax].
        auto root = (h - sqrtd) / a;
        if (root <= tmin || tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= tmin || tmax <= root)
                return false;
        }

        hit.set_t(root);
        hit.set_p(r.at(root));
        hit.set_face_normal(r, (r.at(root) - center) / radius);
        hit.set_shader(shader());

        return true;
    }

private:
    Point3 center;
    float radius;
};

#endif
