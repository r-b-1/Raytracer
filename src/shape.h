#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <utility>

#include "ray.h"
#include "normal_shader.h"
#include "shader.h"
#include "aabb.h"

// Ignore hits this close to the ray origin so a surface cannot intersect the
// ray that spawned it. Every trace (camera and reflection alike) starts here.
inline constexpr float k_min_hit_t = 0.001f;
inline constexpr float k_shadow_eps = 1e-4f;

// Intersection data used by shaders.
class HitStruct {
public:
    Point3 p() const { return p_; }
    Vec3 normal() const { return front_face_ ? outward_normal_ : -outward_normal_; }
    float t() const { return t_; }
    bool front_face() const { return front_face_; }

    const Vec3& ray_direction() const { return ray_dir_; }

    const std::shared_ptr<Shader>& shader() const { return shader_; }

    void set_p(const Point3& p) { p_ = p; }
    void set_t(float t) { t_ = t; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

    void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        // outward_normal must have unit length.
        front_face_ = dot(r.direction(), outward_normal) < 0;
        outward_normal_ = outward_normal;
        ray_dir_ = r.direction();
    }

private:
    Point3 p_ = Point3(0, 0, 0);
    float t_ = 0;
    bool front_face_ = true;
    Vec3 outward_normal_ = Vec3(0, 0, 0);
    Vec3 ray_dir_ = Vec3(0, 0, 0);
    std::shared_ptr<Shader> shader_ = nullptr;
};

class Shape {
public:
    explicit Shape(std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : shader_(std::move(shader)) {}

    virtual ~Shape() = default;

    virtual aabb bounding_box() cosnt = 0;


    const std::shared_ptr<Shader>& shader() const { return shader_; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }


    virtual bool intersect(
        const Ray& r,
        float tmin,
        float& tmax,
        HitStruct& hit
    ) const = 0;

private:
    std::shared_ptr<Shader> shader_;
};

#endif
