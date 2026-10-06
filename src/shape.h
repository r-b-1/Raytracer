#pragma once

#include <memory>
#include <utility>

#include "ray.h"
#include "normal_shader.h"
#include "shader.h"

// Everything the shading stage needs to know about the closest shape hit.
// Shapes fill this in during intersect(); camera::ray_color then hands it
// straight to the shape's Shader.
class HitStruct {
    public:
    point3 p() const {return p_; }
    vec3 normal() const {return front_face_ ? outward_normal_ : -outward_normal_; }
    double t() const { return t_; }
    bool front_face() const {return front_face_; }

    // Direction of the ray that produced this hit. Kept so shaders can build a
    // view direction without Shape having to pass the whole ray along.
    const vec3& ray_direction() const { return ray_dir_; }

    // The material belonging to the shape that was hit. Null if that shape
    // never published one.
    const std::shared_ptr<Shader>& shader() const { return shader_; }

    void set_p(const point3& p) { p_ = p; }
    void set_t(double t) { t_ = t; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // NOTE: `outward_normal` is assumed to have unit length.
        front_face_ = dot(r.direction(), outward_normal) < 0;
        outward_normal_ = outward_normal;
        ray_dir_ = r.direction();
    }
    
    private:
    point3 p_ = point3(0, 0, 0);
    double t_ = 0;
    bool   front_face_ = true;
    vec3   outward_normal_ = vec3(0, 0, 0);
    vec3   ray_dir_ = vec3(0, 0, 0);
    std::shared_ptr<Shader> shader_ = nullptr;
};

// Base for all intersectable geometry. Every shape carries a Shader, which is
// what the renderer dispatches to once a hit is found.
class Shape {
public:
    // Defaults to a NormalShader so a shape constructed without a material
    // still renders (as a normal visualisation) instead of crashing.
    explicit Shape(std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : shader_(std::move(shader)) {}

    virtual ~Shape() = default;

    const std::shared_ptr<Shader>& shader() const { return shader_; }
    void set_shader(std::shared_ptr<Shader> shader) { shader_ = std::move(shader); }

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

private:
    std::shared_ptr<Shader> shader_;
};
