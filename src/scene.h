
#pragma once

#include <memory>
#include <vector>

#include "point_light.h"
#include "shape.h"
#include "point_light.h"

class Scene {
    public:
    std:: vector<std::shared_ptr<Shape>> objects;

    // The scene's light. Shaders are constructed from this so there is one
    // place to move the light when you are testing.
    PointLight light;

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