#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cmath>

#include "shape.h"
#include "vec3.h"

// One triangle, intersected by solving the 3x3 system on slides p.40-41 for the
// three unknowns (beta, gamma, t) and applying Cramer's rule (p.42-43).
class triangle : public Shape {
public:
    triangle(const point3& v0, const point3& v1, const point3& v2,
             std::shared_ptr<Shader> shader = std::make_shared<NormalShader>())
        : Shape(std::move(shader)),
          v0(v0),
          va_minus_vb(v0 - v1),   // the (a, b, c) coefficients, slide p.42
          va_minus_vc(v0 - v2),   // the (d, e, f) coefficients, slide p.42
          n(cross(v1 - v0, v2 - v0)) {
        degenerate = n.length_squared() == 0;
        n = degenerate ? vec3(0, 0, 0) : unit_vector(n);
    }

    bool intersect(const ray& r, double tmin, double& tmax, HitStruct& hit) const override {
        if (degenerate) return false;

        const auto& ro = r.origin();
        const auto& rd = r.direction();

        // Slide p.42: a = (xa-xb), b = (ya-yb), c = (za-zb)
        //              d = (xa-xc), e = (ya-yc), f = (za-zc)
        double a = va_minus_vb.x(), b = va_minus_vb.y(), c = va_minus_vb.z();
        double d = va_minus_vc.x(), e = va_minus_vc.y(), f = va_minus_vc.z();

        // Slide p.42: g = xrd, h = yrd, i = zrd
        double g = rd.x(), h = rd.y(), i = rd.z();

        // The right-hand side is vertex A minus the ray origin (the eye E
        // in the slides). It is unrelated to the matrix coefficient e above.
        double j = v0.x() - ro.x(), k = v0.y() - ro.y(), l = v0.z() - ro.z();

        double M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);
        if (std::fabs(M) < k_parallel_epsilon) return false;  // ray parallel to the plane

        // Barycentric weights locate the point on the triangle's plane:
        // P = (1 - beta - gamma)*v0 + beta*v1 + gamma*v2.
        // We only need them here to check whether P is inside the triangle.
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
    // A parallel ray makes M vanish. This fixed tolerance avoids division by
    // nearly zero; very small geometry may need a scale-aware tolerance later.
    static constexpr double k_parallel_epsilon = 1e-12;

    // One vertex and the two edge differences are enough for the intersection.
    point3 v0;
    vec3 va_minus_vb, va_minus_vc;
    vec3 n;          // unit geometric normal
    bool degenerate; // vertices collinear (or coincident)
};

#endif
