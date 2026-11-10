#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include <cmath>

#include "camera.h"

class PerspectiveCamera : public Camera {
    public:
    // Same viewing frustum the ray tracer uses: an image plane of height
    // image_plane_height placed focal_length in front of the eye.
    glm::mat4 projectionMatrix(float near, float far) const override {
        float fovy = 2.0f * std::atan((image_plane_height / 2.0f) / focal_length);
        return glm::perspective(fovy, aspect_ratio, near, far);
    }

    protected:
    Ray make_ray(const Point3&  pixel_position) const override {
        Vec3 offset = pixel_position - camera_center;

        return Ray(camera_center, offset);
    }
};

#endif
