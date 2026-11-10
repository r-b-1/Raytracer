#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H

#include "camera.h"

class OrthographicCamera : public Camera {
    public:
    // Same viewing box the ray tracer uses: image_plane_height tall,
    // image_plane_height * aspect_ratio wide, centered on the view direction.
    glm::mat4 projectionMatrix(float near, float far) const override {
        float halfHeight = image_plane_height / 2.0f;
        float halfWidth = halfHeight * aspect_ratio;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
    }

    protected:
    Ray make_ray(const Point3& pixel_position) const override {

        return Ray(pixel_position, -w);
    }
};

#endif
