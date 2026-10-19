#include <memory>
#include <vector>

#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"
#include "scene.h"
#include "sphere.h"
#include "rubiks_cube.h"
#include "triangle.h"
#include "mirror_shader.h"


void render_test_scene(const int samples = 100, const int size = 1000) {
    Scene scene;

    const std::vector<PointLight> lights{
        PointLight(Point3(-5, 8, 5), Color(0.7, 0.6, 0.45)),
        PointLight(Point3(6, 6, 2), Color(0.35, 0.45, 0.7)),
    };

    auto grey = std::make_shared<LambertianShader>(Color(0.6, 0.6, 0.6), lights);
    auto yellow = std::make_shared<LambertianShader>(Color(0.9, 0.8, 0.1), lights);
    auto green = std::make_shared<LambertianShader>(Color(0.2, 0.8, 0.2), lights);
    auto teal = std::make_shared<LambertianShader>(Color(0.2, 0.7, 0.7), lights);
    auto purple = std::make_shared<LambertianShader>(Color(0.6, 0.2, 0.8), lights);
    auto mirror = std::make_shared<MirrorShader>(Color(0.8, 0.8, 0.8), lights);
    auto red = std::make_shared<BlinnPhongShader>(
        Color(0.8, 0.1, 0.1), lights, Color(1, 1, 1), 64.0f);
    auto blue = std::make_shared<BlinnPhongShader>(
        Color(0.1, 0.2, 0.8), lights, Color(1, 1, 1), 16.0f);
    auto orange = std::make_shared<BlinnPhongShader>(
        Color(0.95, 0.5, 0.1), lights, Color(1, 1, 1), 32.0f);

    // --- Triangles ---

    // The floor, as two triangles sharing the diagonal from (-50, 0, 50).
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-50, 0, 50), Point3(50, 0, 50), Point3(50, 0, -50), grey));
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-50, 0, 50), Point3(50, 0, -50), Point3(-50, 0, -50), grey));

    // The two mirrored walls at the back of the room.
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-4, 0, -6), Point3(4, 0, -6), Point3(4, 4, -6), mirror));
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-4, 0, -6), Point3(4, 4, -6), Point3(-4, 4, -6), mirror));

    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(0.9, 1.4, -0.6), Point3(1.9, 1.4, -0.6), Point3(1.4, 2.2, -1.1), yellow));

    // --- Spheres ---

    scene.objects.push_back(std::make_shared<Sphere>(Point3(-2, 0.8, -3), 0.8, green));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(-1.1, 0.6, -1.2), 0.6, red));
    scene.objects.push_back(std::make_shared<Sphere>(
        Point3(0, 0.9, -3.5), 0.9, std::make_shared<NormalShader>()));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(0.3, 0.45, -1.6), 0.45, blue));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(2, 1, -3), 1, mirror));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(1.5, 0.5, 0.3), 0.5, mirror));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(-1.3, 0.45, 0.4), 0.45, teal));

    // Behind the camera: only ever seen in the mirrors.
    scene.objects.push_back(std::make_shared<Sphere>(Point3(0.3, 1, 5), 1, orange));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(2.4, 0.8, 4.8), 0.8, purple));

    PerspectiveCamera pcam;
    pcam.lookfrom = Point3(0, 3, 3);
    pcam.lookat = Point3(0, 2.6, 2); // lookfrom + the specified view direction
    pcam.focal_length = 0.4;
    pcam.image_plane_height = 0.5;
    pcam.max_depth = 5;
    pcam.samples_per_pixel = samples;
    pcam.image_width = size;
    pcam.image_height = size;

    pcam.render(scene, "scene_perspective.png");

    OrthographicCamera ocam;
    ocam.lookfrom = Point3(0, 3, 3);
    ocam.lookat = Point3(0, 2.6, 2); // lookfrom + the specified view direction
    ocam.focal_length = 0.4;
    ocam.image_plane_height = 5;
    ocam.max_depth = 5;
    ocam.samples_per_pixel = samples;
    ocam.image_width = size;
    ocam.image_height = size;

    ocam.render(scene, "scene_orthographic.png");
}

int main() {
    Scene scene;
    scene.light.set_position(Point3(0, 10, 5));

    auto matte = std::make_shared<LambertianShader>(Color(0.95, 0.95, 0.15), scene.light);
    auto shiny = std::make_shared<BlinnPhongShader>(Color(0.95, 0.95, 0.15),
                                                   Color(1, 1, 1),
                                                   64.0f,
                                                   0.8f,
                                                   scene.light);

    scene.objects.push_back(std::make_shared<Sphere>(Point3(-0.7, 0, -1), 0.5, matte));
    scene.objects.push_back(std::make_shared<Sphere>(Point3(0.7, 0, -1), 0.5, shiny));

    PerspectiveCamera cam;
    cam.lookfrom = Point3(0, 0, 0);
    cam.lookat = Point3(0, 0, -1.5);

    cam.samples_per_pixel = 10;
    
    cam.render(scene, "shaded.png");

    Scene lambert_scene;
    lambert_scene.light = scene.light;
    lambert_scene.objects.push_back(
        std::make_shared<Sphere>(Point3(0, 0, -1), 0.6, std::make_shared<LambertianShader>(Color(0.95, 0.95, 0.15), scene.light)));
    cam.render(lambert_scene, "lambertian.png");

    Scene mirror_scene;
    mirror_scene.light = scene.light;

    // A flat mirror as the floor. A mirror *sphere* only reflects a ~53 degree
    // cone around the camera axis, so it can show nothing but sky and ground;
    // a flat one reflects the objects resting on it.
    mirror_scene.objects.push_back(
        std::make_shared<Triangle>(Point3(-20, 0, -20), Point3(20, 0, -20), Point3(0, 0, 20),
                                   std::make_shared<MirrorShader>(Color(0.85, 0.85, 0.85), mirror_scene.light)));
    mirror_scene.objects.push_back(std::make_shared<Sphere>(Point3(-0.6, 1.2, -0.5), 0.55, matte));
    mirror_scene.objects.push_back(std::make_shared<Sphere>(Point3(0.9, 0.6, 0.3), 0.4, shiny));

    PerspectiveCamera mirror_cam;
    mirror_cam.lookfrom = Point3(0, 2.0, 3.5);
    mirror_cam.lookat = Point3(0, 0.6, -0.5);
    mirror_cam.render(mirror_scene, "mirror.png");

    Scene phong_scene;
    phong_scene.light = scene.light;
    phong_scene.objects.push_back(
        std::make_shared<Sphere>(Point3(0, 0, -1), 0.6, std::make_shared<BlinnPhongShader>(Color(0.95, 0.95, 0.15), Color(1, 1, 1), 64.0f, 0.8f, scene.light)));
    cam.render(phong_scene, "blinn_phong.png");

    Scene snowman_scene;
    snowman_scene.light = scene.light;

    auto snow = std::make_shared<BlinnPhongShader>(
        Color(0.95, 0.95, 0.95), Color(1, 1, 1), 64.0f, 0.5f, snowman_scene.light);
    auto coal = std::make_shared<LambertianShader>(
        Color(0.03, 0.03, 0.03), snowman_scene.light);
    auto carrot = std::make_shared<BlinnPhongShader>(
        Color(1.0, 0.35, 0.05), Color(1, 1, 1), 32.0f, 0.4f, snowman_scene.light);
    auto ground = std::make_shared<LambertianShader>(
        Color(0.25, 0.45, 0.15), snowman_scene.light);
    auto sun = std::make_shared<BlinnPhongShader>(
        Color(1.0, 0.9, 0.15), Color(1, 1, 1), 1.0f, 2.0f, snowman_scene.light);

    snowman_scene.objects.push_back(
        std::make_shared<Sphere>(Point3(0, -100.5, -1), 100, ground));

    // Body
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 0, -1), 0.5, snow));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 0.75, -1), 0.4, snow));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 1.25, -1), 0.27, snow));

    // Buttons
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 0.9, -0.73), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 0.75, -0.69), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 0.6, -0.73), 0.1, coal));

    // Eyes and nose
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0.2, 1.33, -0.8), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(-0.2, 1.33, -0.8), 0.1, coal));
    snowman_scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 1.2, -0.83), 0.1, carrot));

    snowman_scene.objects.push_back(
        std::make_shared<Sphere>(Point3(0, 100.5, -200), 100, sun));

    PerspectiveCamera snowman_cam;
    snowman_cam.lookfrom = Point3(0, 0.8, 1);
    snowman_cam.lookat = Point3(0, 0.65, -1);
    snowman_cam.render(snowman_scene, "snowman.png");

    PerspectiveCamera rubiks_cam;
    // View the top, front, and right faces.
    rubiks_cam.lookfrom = Point3(3, 5, 3);
    rubiks_cam.lookat = Point3(0, 1.5, 0);

    Scene rubiks_scene;
    // This light serves the base and hidden faces; visible tiles have highlight lights.
    rubiks_scene.light.set_position(rubiks_cam.lookfrom + Vec3(0, 1, 0));

    auto black = std::make_shared<LambertianShader>(
        Color(0.03, 0.03, 0.03), rubiks_scene.light);

    auto grey = std::make_shared<LambertianShader>(
        Color(0.25, 0.25, 0.25), rubiks_scene.light);

    add_rubiks_cube(rubiks_scene, black);
    rubiks_scene.objects.push_back(std::make_shared<Triangle>(Point3(1000, 0, -1000), Point3(-1000, 0, -1000), Point3(0, 0, 1000), grey));

    rubiks_cam.render(rubiks_scene, "rubiks.png");

//     OrthographicCamera rubiks_orth_cam;
// //     rubiks_cam.lookfrom = Point3(1, 2, 5);
//     rubiks_orth_cam.lookfrom = Point3(1, 2, 5);
//     rubiks_orth_cam.lookat = Point3(0, 1, 4);
//     rubiks_orth_cam.render(rubiks_scene, "rubiks_orth.png");

    render_test_scene();

}


// TODO: Make a rubiks cube, base black with colored 3x3 tiles. 
// Note: it would also be cool that the cube was the scramble for the current world record solve.
