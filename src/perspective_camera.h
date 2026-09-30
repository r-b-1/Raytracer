
#pragma once

#include "camera.h"

class PerspectiveCamera : public camera {
    protected:
    ray make_ray(const point3&  pixel_position) const override {
        vec3 offset = pixel_position - camera_center;

        return ray(camera_center, offset - w);
    }
};