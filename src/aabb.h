#ifndef AABB_H
#define AABB_H

#include <limits>

#include "ray.h"

// One axis of a box. Empty by default so an untouched box contains nothing.
class interval {
public:
    float min;
    float max;

    interval()
        : min(std::numeric_limits<float>::infinity()),
          max(-std::numeric_limits<float>::infinity()) {}

    interval(float min, float max) : min(min), max(max) {}

    // Tight interval around both inputs. An empty input disappears.
    interval(const interval& a, const interval& b)
        : min(std::fmin(a.min, b.min)), max(std::fmax(a.max, b.max)) {}

    float size() const { return max - min; }

    interval expand(float delta) const {
        const float padding = delta / 2.0f;
        return interval(min - padding, max + padding);
    }
};

class aabb {
public:
    interval x, y, z;

    aabb() {} // The default AABB is empty because intervals are empty by default

    aabb(const interval& x, const interval& y, const interval& z)
        : x(x), y(y), z(z) {}

    // Order each axis so min <= max, even if the two corners are swapped.
    aabb(const Point3& a, const Point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    // Smallest box that contains both boxes.
    aabb(const aabb& a, const aabb& b)
        : x(interval(a.x, b.x)), y(interval(a.y, b.y)), z(interval(a.z, b.z)) {}

    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const Ray& r, interval ray_t) const {
        const Point3& ray_orig = r.origin();
        const Vec3& ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);
            const float adinv = 1.0f / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    // Index of the widest side: 0 for x, 1 for y, 2 for z.
    int longest_axis() const {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty;

    // A flat face has no thickness, so give any thin axis a little padding.
    aabb pad_to_minimums() const {
        constexpr float delta = 0.0001f;
        return aabb(
            (x.size() >= delta) ? x : x.expand(delta),
            (y.size() >= delta) ? y : y.expand(delta),
            (z.size() >= delta) ? z : z.expand(delta));
    }
};

inline const aabb aabb::empty = aabb(interval(), interval(), interval());

#endif


// Simple idea of what we are doing here:
// if (ray hits bounding object)
//      return wether ray hits bounded objects
// else
//      return false
