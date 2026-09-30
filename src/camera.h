#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "framebuffer.h"

#include "scene.h"
// #include "ray.h"

class camera {
    public:
    float aspect_ratio = 1;
    int image_width = 500;
    int image_height = 500;
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);



    // One render visits every shape in the scene for each camera ray.
    void render(const Scene& scene, const std::string& filename = "image.png") {
        initialize();
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                auto u = double(i);
                auto v = double(j);
                auto pixel_color = ray_color(ray(camera_center,
                                                 pixel00_loc + u*pixel_delta_u
                                                 + v*pixel_delta_v - camera_center),
                                             scene);
                framebuffer.setPixel(i, j, pixel_color);
            }
        }
        framebuffer.exportAsPNG(filename);
    }

    private:

    color ray_color(const ray& r, const Scene& scene) const {
        double tmax = std::numeric_limits<double>::infinity();
        HitStruct rec;
        if (scene.intersect(r, k_tmin, tmax, rec)) {
            return 0.5 * (rec.normal() + color(1, 1, 1));
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    // Nearest hit the camera will accept. The week 4 slides suggest 1.0, to
    // reject geometry at or in front of the image plane, but this scene's
    // sphere sits at t ~= 0.5, so that would blank the whole image. A small
    // epsilon just stops a ray from registering the surface it starts on.
    static constexpr double k_tmin = 0.001;

    Framebuffer framebuffer;


    void initialize() {
        // Calculate the image height to ensure that its at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        framebuffer = Framebuffer(image_width, image_height);

        camera_center = lookfrom;

        vec3 w = unit_vector(lookfrom - lookat); // Camera backward
        vec3 u = unit_vector(cross(vup, w));     // Camera right
        vec3 v = cross(w, u);                   // Camera up

        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Across the image: camera right.
        // Down the image: negative camera up, matching PNG row order.
        auto viewport_u = viewport_width * u;
        auto viewport_v = -viewport_height * v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left =
            camera_center - focal_length * w
            - viewport_u / 2
            - viewport_v / 2;

        pixel00_loc =
            viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    }

    protected:
    point3 camera_center;
    vec3 w;
    
    point3 pixel00_loc;
    vec3 pixel_delta_u, pixel_delta_v;

    virtual ray make_ray(const point3& pixel_position) const = 0;
};

#endif
