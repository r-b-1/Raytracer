#include <memory>

#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"
#include "scene.h"
#include "sphere.h"
#include "rubiks_cube.h"

int main() {
    Scene scene;
    scene.light.set_position(point3(0, 10, 5));

    auto matte = std::make_shared<LambertianShader>(color(0.95, 0.95, 0.15), scene.light);
    auto shiny = std::make_shared<BlinnPhongShader>(color(0.95, 0.95, 0.15),
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

    Scene lambert_scene;
    lambert_scene.light = scene.light;
    lambert_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, 0, -1), 0.6, std::make_shared<LambertianShader>(color(0.95, 0.95, 0.15), scene.light)));
    cam.render(lambert_scene, "lambertian.png");

    Scene phong_scene;
    phong_scene.light = scene.light;
    phong_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, 0, -1), 0.6, std::make_shared<BlinnPhongShader>(color(0.95, 0.95, 0.15), color(1, 1, 1), 64.0f, 0.8f, scene.light)));
    cam.render(phong_scene, "blinn_phong.png");

    Scene snowman_scene;
    snowman_scene.light = scene.light;

    auto snow = std::make_shared<BlinnPhongShader>(
        color(0.95, 0.95, 0.95), color(1, 1, 1), 64.0f, 0.5f, snowman_scene.light);
    auto coal = std::make_shared<LambertianShader>(
        color(0.03, 0.03, 0.03), snowman_scene.light);
    auto carrot = std::make_shared<BlinnPhongShader>(
        color(1.0, 0.35, 0.05), color(1, 1, 1), 32.0f, 0.4f, snowman_scene.light);
    auto ground = std::make_shared<LambertianShader>(
        color(0.25, 0.45, 0.15), snowman_scene.light);
    auto sun = std::make_shared<BlinnPhongShader>(
        color(1.0, 0.9, 0.15), color(1, 1, 1), 1.0f, 2.0f, snowman_scene.light);

    snowman_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, -100.5, -1), 100, ground));

    // Body
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 0, -1), 0.5, snow));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 0.75, -1), 0.4, snow));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 1.25, -1), 0.27, snow));

    // Buttons
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 0.9, -0.73), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 0.75, -0.69), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 0.6, -0.73), 0.1, coal));

    // Eyes and nose
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0.2, 1.33, -0.8), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(-0.2, 1.33, -0.8), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<sphere>(point3(0, 1.2, -0.83), 0.1, carrot));

    snowman_scene.objects.push_back(
        std::make_shared<sphere>(point3(0, 100.5, -200), 100, sun));

    PerspectiveCamera snowman_cam;
    snowman_cam.lookfrom = point3(0, 0.8, 1);
    snowman_cam.lookat = point3(0, 0.65, -1);
    snowman_cam.render(snowman_scene, "snowman.png");

    PerspectiveCamera rubiks_cam;
    // View the top, front, and right faces.
    rubiks_cam.lookfrom = point3(3, 5, 3);
    rubiks_cam.lookat = point3(0, 1.5, 0);

    Scene rubiks_scene;
    // This light serves the base and hidden faces; visible tiles have highlight lights.
    rubiks_scene.light.set_position(rubiks_cam.lookfrom + vec3(0, 1, 0));

    auto black = std::make_shared<LambertianShader>(
        color(0.03, 0.03, 0.03), rubiks_scene.light);

    add_rubiks_cube(rubiks_scene, black);

    rubiks_cam.render(rubiks_scene, "rubiks.png");

//     OrthographicCamera rubiks_orth_cam;
// //     rubiks_cam.lookfrom = point3(1, 2, 5);
//     rubiks_orth_cam.lookfrom = point3(1, 2, 5);
//     rubiks_orth_cam.lookat = point3(0, 1, 4);
//     rubiks_orth_cam.render(rubiks_scene, "rubiks_orth.png");
}


// TODO: Make a rubiks cube, base black with colored 3x3 tiles. 
// Note: it would also be cool that the cube was the scramble for the current world record solve.
