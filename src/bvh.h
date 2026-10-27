#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <memory>
#include <vector>

#include "shape.h"

// Binary tree of shapes. A ray that misses a node's box skips everything inside it.
class BvhNode : public Shape {
public:
    explicit BvhNode(std::vector<std::shared_ptr<Shape>>& objects)
        : BvhNode(objects, 0, objects.size()) {}

    BvhNode(std::vector<std::shared_ptr<Shape>>& objects, size_t start, size_t end) {
        bbox = aabb::empty;
        for (size_t i = start; i < end; ++i)
            bbox = aabb(bbox, objects[i]->bounding_box());

        const int axis = bbox.longest_axis();
        const auto comparator =
            [axis](const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
                return a->bounding_box().axis_interval(axis).min
                     < b->bounding_box().axis_interval(axis).min;
            };

        const size_t span = end - start;
        if (span == 1) {
            left = right = objects[start];
        } else if (span == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            std::sort(objects.begin() + static_cast<std::ptrdiff_t>(start),
                      objects.begin() + static_cast<std::ptrdiff_t>(end),
                      comparator);
            const size_t mid = start + span / 2;
            left = std::make_shared<BvhNode>(objects, start, mid);
            right = std::make_shared<BvhNode>(objects, mid, end);
        }
    }

    aabb bounding_box() const override { return bbox; }

    bool intersect(const Ray& r, float tmin, float& tmax, HitStruct& hit) const override {
        if (!bbox.hit(r, interval(tmin, tmax)))
            return false;

        const bool hit_left = left->intersect(r, tmin, tmax, hit);
        if (hit_left)
            tmax = hit.t();
        const bool hit_right = right->intersect(r, tmin, tmax, hit);
        return hit_left || hit_right;
    }

private:
    std::shared_ptr<Shape> left;
    std::shared_ptr<Shape> right;
    aabb bbox;
};

// Replace the flat list with one root node. The node keeps the shapes alive.
inline void build_bvh(std::vector<std::shared_ptr<Shape>>& objects) {
    if (objects.empty())
        return;
    auto root = std::make_shared<BvhNode>(objects);
    objects.clear();
    objects.push_back(std::move(root));
}

#endif
