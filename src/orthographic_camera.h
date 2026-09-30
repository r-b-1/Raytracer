
#pragma once

#include "camera.h"

class OrthographicCamera : public camera {
    protected:
    ray make_ray(const point3& pixel_position) const override {

        return ray(pixel_position, -w);
    }
};