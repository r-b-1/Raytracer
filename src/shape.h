#pragma once

#include <memory>
#include <utility>

#include "ray.h"
#include "normal_shader.h"
#include "shader.h"

// Intersection data used by shaders.
class HitStruct {
public:
    point3 p() const { return p_; }
    vec3 normal() const { return front_face_ ? outward_normal_ : -outward_normal_; }
    double t() const { return t_; }
    bool front_face() const { return front_face_; }

    const vec3& ray_direction() const { return ray_dir_; }

    const std::shared_ptr<Shader>& shader() const { return shader_; }

    void set_p(const point3& p) { p_ = p; }
    void set_t(double t) { t_ = t; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // outward_normal must have unit length.
        front_face_ = dot(r.direction(), outward_normal) < 0;
        outward_normal_ = outward_normal;
        ray_dir_ = r.direction();
    }

private:
    point3 p_ = point3(0, 0, 0);
    double t_ = 0;
    bool front_face_ = true;
    vec3 outward_normal_ = vec3(0, 0, 0);
    vec3 ray_dir_ = vec3(0, 0, 0);
    std::shared_ptr<Shader> shader_ = nullptr;
};

class Shape {
public:
    explicit Shape(std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : shader_(std::move(shader)) {}

    virtual ~Shape() = default;

    const std::shared_ptr<Shader>& shader() const { return shader_; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

    // Fill hit on success; leave it untouched on a miss. Scene updates tmax.
    virtual bool intersect(
        const ray& r,
        double tmin,
        double& tmax,
        HitStruct& hit
    ) const = 0;

private:
    std::shared_ptr<Shader> shader_;
};
