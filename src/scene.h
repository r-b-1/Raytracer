#pragma once

#include <memory>
#include <vector>

#include "point_light.h"
#include "shape.h"

class Scene {
public:
    std::vector<std::shared_ptr<Shape>> objects;

    PointLight light;

    bool intersect(const ray& r, double tmin, double& tmax, HitStruct& hit) const {
        bool found = false;

        for (const auto& object : objects) {
            HitStruct candidate;

            if (object->intersect(r, tmin, tmax, candidate)) {
                tmax = candidate.t();
                hit = candidate;
                found = true;
            }
        }

        return found;
    }
};
