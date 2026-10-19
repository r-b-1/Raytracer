#ifndef CAMERA_H
#define CAMERA_H

#include <atomic>
#include <iostream>

#include "color.h"
#include "framebuffer.h"

#include "scene.h"

class Camera {
public:
    float aspect_ratio = 1;
    int image_width = 1000;
    int image_height = 1000;
    Point3 lookfrom = Point3(0, 0, 0);
    Point3 lookat = Point3(0, 0, 0);
    Vec3 vup = Vec3(0, 1, 0);
    int samples_per_pixel = 10;
    int max_depth = 50; // Maximum reflection bounces per ray.

    // Projection: the image plane sits `focal_length` in front of the eye and
    // spans `image_plane_height` there, so a half-height of image_plane_height
    // / 2 subtends atan(image_plane_height / 2 / focal_length).
    float focal_length = 1.0;
    float image_plane_height = 2.0;

    // Rows between progress updates. Too small and the terminal is flooded;
    // too large and the bar stalls between frames.
    int progress_interval = 10;

    void render(const Scene& scene, const std::string& filename = "image.png") {
        initialize();

        // The render loop is parallel, so completion is counted rather than
        // reported per iteration; each thread bumps the same counter.
        std::atomic<int> rows_done{0};
        const int total_rows = image_height;

        std::clog << "\rRendering rows: 0/" << total_rows << " (0%)  " << std::flush;

        #pragma omp parallel for
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                Color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; s++) {
                    pixel_color += trace(scene, get_ray(i, j), max_depth);
                }
                pixel_color *= pixel_samples_scale;
                framebuffer.setPixel(i, j, pixel_color);
            }

            const int done = rows_done.fetch_add(1) + 1;
            if (done % progress_interval == 0 || done == total_rows) {
                // Writes are flushed without a newline so the bar overwrites in
                // place; the newline below ends it once encoding is done.
                #pragma omp critical
                {
                    const int percent = (100 * done) / total_rows;
                    std::clog << "\rRendering rows: " << done << '/' << total_rows
                              << " (" << percent << "%)  " << std::flush;
                }
            }
        }
        std::clog << '\n';

        framebuffer.exportAsPNG(filename);
    }

private:
    float pixel_samples_scale;
    Framebuffer framebuffer;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        framebuffer = Framebuffer(image_width, image_height);

        camera_center = lookfrom;

        w = unit_vector(lookfrom - lookat);  // Camera backward
        Vec3 u = unit_vector(cross(vup, w)); // Camera right
        Vec3 v = cross(w, u);               // Camera up

        pixel_samples_scale = 1.0f / static_cast<float>(samples_per_pixel);
        auto viewport_height = float(image_plane_height);
        auto viewport_width = viewport_height * (float(image_width) / image_height);

        auto viewport_u = viewport_width * u;
        // PNG rows run from top to bottom.
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

    Ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        return make_ray(pixel_sample);
    }

    Vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(random_float() - 0.5, random_float() - 0.5, 0);
    }    

protected:
    Point3 camera_center;
    Vec3 w;

    Point3 pixel00_loc;
    Vec3 pixel_delta_u, pixel_delta_v;

    virtual Ray make_ray(const Point3& pixel_position) const = 0;
};

#endif
