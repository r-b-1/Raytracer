
#pragma once

#include <memory>
#include <vector>

#include "shape.h"

class Scene {
    public:
    std:: vector<std::shared_ptr<Shape>> objects;

    bool intersect(const ray& r, double tmin, double& tmax, HitStruct& hit) const {
        bool found = false;

        for (const auto& object : objects) {
            HitStruct canidate;

            if (object->intersect(r, tmin, tmax, canidate)) {
                tmax = canidate.t();
                hit = canidate;
                found = true;
            }
        }

        return found;
        
    }
};