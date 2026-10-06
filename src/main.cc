#include "rtweekend.h"

#include "hittable.h"
#include "sphere.h"
#include "scene.h"

#include "camera.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"

#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "normal_shader.h"

int main() {
    Scene scene;
    scene.light.set_position(point3(0, 10, 5));

    // One matte sphere and one shiny sphere, so the two shading models can be
    // compared directly in a single image.
    auto matte = std::make_shared<LambertianShader>(color(0.85, 0.25, 0.25), scene.light);
    auto shiny = std::make_shared<BlinnPhongShader>(color(0.25, 0.45, 0.85),
                                                   color(1, 1, 1),
                                                   64.0f,
                                                   0.8f,
                                                   scene.light);

    scene.objects.push_back(std::make_shared<sphere>(point3(-0.7, 0, -1), 0.5, matte));
    scene.objects.push_back(std::make_shared<sphere>(point3(0.7, 0, -1), 0.5, shiny));

    PerspectiveCamera cam;
    cam.lookfrom = point3(0, 0, 0);
    cam.lookat = point3(0, 0, -1.5);
    cam.render(scene, "shaded.png");

    // Second deliverable: each shading model on its own, one sphere per image.
    Scene lambert_scene;
    lambert_scene.light = scene.light;
    lambert_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, 0, -1), 0.6, std::make_shared<LambertianShader>(color(0.85, 0.25, 0.25), scene.light)));
    cam.render(lambert_scene, "lambertian.png");

    Scene phong_scene;
    phong_scene.light = scene.light;
    phong_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, 0, -1), 0.6, std::make_shared<BlinnPhongShader>(color(0.25, 0.45, 0.85), color(1, 1, 1), 64.0f, 0.8f, scene.light)));
    cam.render(phong_scene, "blinn_phong.png");
}
