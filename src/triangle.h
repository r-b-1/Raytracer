#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cmath>

#include "shape.h"
#include "vec3.h"

// Triangle intersection using Cramer's rule.
class triangle : public Shape {
public:
    triangle(const point3& v0, const point3& v1, const point3& v2,
             std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : Shape(std::move(shader)),
          v0(v0),
          va_minus_vb(v0 - v1),
          va_minus_vc(v0 - v2),
          n(cross(v1 - v0, v2 - v0)) {
        degenerate = n.length_squared() == 0;
        n = degenerate ? vec3(0, 0, 0) : unit_vector(n);
    }

    bool intersect(const ray& r, double tmin, double& tmax, HitStruct& hit) const override {
        if (degenerate) return false;

        const auto& ro = r.origin();
        const auto& rd = r.direction();

        // Matrix columns: triangle edges and ray direction.
        double a = va_minus_vb.x(), b = va_minus_vb.y(), c = va_minus_vb.z();
        double d = va_minus_vc.x(), e = va_minus_vc.y(), f = va_minus_vc.z();

        double g = rd.x(), h = rd.y(), i = rd.z();

        // Right-hand side: first vertex minus ray origin.
        double j = v0.x() - ro.x(), k = v0.y() - ro.y(), l = v0.z() - ro.z();

        double M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);
        if (std::fabs(M) < k_parallel_epsilon) return false;  // ray parallel to the plane

        // Barycentric coordinates: P = (1 - beta - gamma)*v0 + beta*v1 + gamma*v2.
        double beta  = (j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g)) / M;
        double gamma = (i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c)) / M;

        // Cramer's rule for t: replace the matrix's third column with (j,k,l).
        double t = (a*(e*l - f*k) - d*(b*l - k*c) + j*(b*f - e*c)) / M;

        if (t < tmin || t > tmax) return false;
        if (gamma < 0 || gamma > 1) return false;
        if (beta < 0 || beta > 1 - gamma) return false;

        hit.set_t(t);
        hit.set_p(r.at(t));
        hit.set_face_normal(r, n);
        hit.set_shader(shader());

        return true;
    }

private:
    // Reject nearly parallel rays before dividing by the determinant.
    static constexpr double k_parallel_epsilon = 1e-12;

    point3 v0;
    vec3 va_minus_vb, va_minus_vc;
    vec3 n;          // unit geometric normal
    bool degenerate; // vertices collinear (or coincident)
};

#endif
