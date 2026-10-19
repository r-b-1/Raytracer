#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H

#include "camera.h"

class OrthographicCamera : public Camera {
    protected:
    Ray make_ray(const Point3& pixel_position) const override {

        return Ray(pixel_position, -w);
    }
};

#endif
