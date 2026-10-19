#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include "camera.h"

class PerspectiveCamera : public Camera {
    protected:
    Ray make_ray(const Point3&  pixel_position) const override {
        Vec3 offset = pixel_position - camera_center;

        return Ray(camera_center, offset);
    }
};

#endif
