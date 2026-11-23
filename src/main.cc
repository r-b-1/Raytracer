#include <iostream>
#include <memory>
#include <vector>

#include "handleGraphicsArgs.h"
#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "mirror_shader.h"
#include "rubiks_cube.h"

#include "aabb.h"
#include "bvh.h"
#include "obj_mesh.h"

#include "SceneParser_JSON.h"
#include "SceneLoader.h"

void render_test_scene(int image_width, float aspect_ratio, int samples) {
    Scene scene;

    const std::vector<std::shared_ptr<Light>> lights{
        std::make_shared<PointLight>(Point3(-5, 8, 5), Color(0.7, 0.6, 0.45)),
        std::make_shared<PointLight>(Point3(6, 6, 2), Color(0.35, 0.45, 0.7)),
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

    build_bvh(scene.objects);

    PerspectiveCamera pcam;
    pcam.lookfrom = Point3(0, 3, 3);
    pcam.lookat = Point3(0, 2.6, 2); // lookfrom + the specified view direction
    pcam.focal_length = 0.4;
    pcam.image_plane_height = 0.5;
    pcam.max_depth = 5;
    pcam.samples_per_pixel = samples;
    // Height is derived from width and aspect ratio inside Camera::initialize.
    pcam.image_width = image_width;
    pcam.aspect_ratio = aspect_ratio;

    pcam.render(scene, "scene_perspective.png");

    OrthographicCamera ocam;
    ocam.lookfrom = Point3(0, 3, 3);
    ocam.lookat = Point3(0, 2.6, 2); // lookfrom + the specified view direction
    ocam.focal_length = 0.4;
    ocam.image_plane_height = 5;
    ocam.max_depth = 5;
    ocam.samples_per_pixel = samples;
    ocam.image_width = image_width;
    ocam.aspect_ratio = aspect_ratio;

    ocam.render(scene, "scene_orthographic.png");
}

// Two snowmen built from spheres, reusing the model from the ray tracer's
// snowman scene. `base` is the center of the bottom snowball.
void add_snowman(Scene& scene, const Point3& base,
                 const std::shared_ptr<Shader>& snow,
                 const std::shared_ptr<Shader>& coal,
                 const std::shared_ptr<Shader>& carrot) {
    scene.objects.push_back(std::make_shared<Sphere>(base, 0.75f, snow));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 1.125f, 0), 0.60f, snow));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 1.875f, 0), 0.405f, snow));

    // Buttons on the middle snowball, facing the camera (+z). Sized to the
    // same proportion as the ray tracer's original snowman (0.1 on a 0.4 ball).
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 0.90f, 0.556f), 0.09f, coal));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 1.125f, 0.60f), 0.09f, coal));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 1.35f, 0.556f), 0.09f, coal));

    // Big eyes and a small carrot nose on the head.
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(-0.30f, 1.975f, 0.28f), 0.17f, coal));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0.30f, 1.975f, 0.28f), 0.17f, coal));
    scene.objects.push_back(std::make_shared<Sphere>(base + Point3(0, 1.82f, 0.40f), 0.10f, carrot));
}

// The scene used for the ray tracer / OpenGL side-by-side comparison. The
// OpenGL app draws the same geometry, camera, light, and colors.
void render_comparison_scene(int image_width, float aspect_ratio, int samples) {
    Scene scene;
    scene.light.set_position(Point3(0, 6, 5));
    scene.light.set_intensity(Color(1, 1, 1));

    auto floor = std::make_shared<LambertianShader>(
        Color(0.45f, 0.45f, 0.42f), scene.light);
    auto bunny = std::make_shared<BlinnPhongShader>(
        Color(0.8f, 0.3f, 0.8f), Color(1, 1, 1), 32.0f, 1.0f, scene.light);
    auto sun = std::make_shared<BlinnPhongShader>(
        Color(1.0f, 0.9f, 0.15f), Color(1, 1, 1), 32.0f, 1.0f, scene.light);
    auto snow = std::make_shared<BlinnPhongShader>(
        Color(0.95f, 0.95f, 0.95f), Color(1, 1, 1), 32.0f, 1.0f, scene.light);
    auto coal = std::make_shared<BlinnPhongShader>(
        Color(0.03f, 0.03f, 0.03f), Color(1, 1, 1), 32.0f, 1.0f, scene.light);
    auto carrot = std::make_shared<BlinnPhongShader>(
        Color(1.0f, 0.35f, 0.05f), Color(1, 1, 1), 32.0f, 1.0f, scene.light);

    // Floor at y = -3: the bunny's feet and the snowmen touch it.
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-100, -3, 100), Point3(100, -3, 100), Point3(100, -3, -100), floor));
    scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-100, -3, 100), Point3(100, -3, -100), Point3(-100, -3, -100), floor));

    scene.objects.push_back(
        load_obj_mesh("assets/bunny.obj", bunny, Point3(0, 0, 0), 6.0f));

    add_snowman(scene, Point3(-2.9f, -2.25f, 2.2f), snow, coal, carrot);
    add_snowman(scene, Point3(2.9f, -2.25f, 2.2f), snow, coal, carrot);

    // Sun: the same huge background sphere used in the snowman scene.
    scene.objects.push_back(std::make_shared<Sphere>(Point3(0, 100, -200), 100, sun));

    build_bvh(scene.objects);

    PerspectiveCamera cam;
    cam.lookfrom = Point3(0, 0, 10);
    cam.lookat = Point3(0, 0, 0);
    cam.image_plane_height = 2.0f;
    cam.focal_length = 2.0f;
    cam.max_depth = 1; // Direct lighting only.
    cam.samples_per_pixel = samples;
    cam.image_width = image_width;
    cam.aspect_ratio = aspect_ratio;
    cam.render(scene, "raytrace.png");
}

int main(int argc, char* argv[]) {
    sivelab::GraphicsArgs args;
    try {
        args.process(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        args.printUsage();
        return 1;
    }

    // if (argc < 2) {
    //     std::cerr << "Usage ./main scene.json\n";
    //     return 1;
    // }

    // Scene scene;
    // auto loader = std::make_shared<SceneLoader>(scene);
    // SceneParser_JSON parser(loader);
    // parser.parseFileData(argv[1]);

    // if (scene.cameras.empty()) {
    //     std::cerr << "Scenes need to have atleast 1 camera.\n";
    //     return 1;
    // }

    // scene.cameras.front()->render(scene, "image.png");

    // 1000x1000 unless the command line says otherwise. Passing only one
    // dimension keeps the image square, matching the cameras' default aspect.
    int image_width = 1000;
    int image_height = 1000;
    const bool width_set = args.isSet("width", image_width);
    const bool height_set = args.isSet("height", image_height);
    if (width_set && !height_set)
        image_height = image_width;
    else if (height_set && !width_set)
        image_width = image_height;

    if (image_width < 1 || image_height < 1) {
        std::cerr << "Image width and height must be at least 1.\n";
        return 1;
    }

    float aspect_ratio = static_cast<float>(image_width) / static_cast<float>(image_height);
    if (args.isSet("aspect", aspect_ratio) && aspect_ratio <= 0.0f) {
        std::cerr << "Aspect ratio must be positive.\n";
        return 1;
    }

    int rpp = 1;
    const bool rpp_set = args.isSet("rpp", rpp);
    if (rpp_set && rpp < 1) {
        std::cerr << "Rays per pixel must be at least 1.\n";
        return 1;
    }

    // Ray tracer half of the side-by-side comparison (see README).
    try {
        render_comparison_scene(image_width, aspect_ratio, rpp_set ? rpp : 16);
    } catch (const std::exception& e) {
        std::cerr << "Skipping comparison scene: " << e.what()
                  << " (run from the project root)\n";
    }

    Scene snowman_scene;
    snowman_scene.light.set_position(Point3(0, 10, 5));

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

    build_bvh(snowman_scene.objects);

    PerspectiveCamera snowman_cam;
    snowman_cam.lookfrom = Point3(0, 0.8, 1);
    snowman_cam.lookat = Point3(0, 0.65, -1);
    snowman_cam.image_width = image_width;
    snowman_cam.aspect_ratio = aspect_ratio;
    if (rpp_set)
        snowman_cam.samples_per_pixel = rpp;
    snowman_cam.render(snowman_scene, "snowman.png");

    PerspectiveCamera rubiks_cam;
    // Top, front, and right faces.
    rubiks_cam.lookfrom = Point3(3, 5, 3);
    rubiks_cam.lookat = Point3(0, 1.5, 0);
    rubiks_cam.max_depth = 5;
    rubiks_cam.image_width = image_width;
    rubiks_cam.aspect_ratio = aspect_ratio;
    if (rpp_set)
        rubiks_cam.samples_per_pixel = rpp;

    Scene rubiks_scene;
    // Overhead, toward the camera, so the floor and the three near faces read bright.
    // The far faces point the other way, so they stay dark.
    rubiks_scene.light.set_position(Point3(2, 10, 2));
    rubiks_scene.light.set_intensity(Color(1.8f, 1.8f, 1.8f));

    auto black = std::make_shared<LambertianShader>(
        Color(0.03, 0.03, 0.03), rubiks_scene.light);
    auto floor = std::make_shared<LambertianShader>(
        Color(0.55, 0.55, 0.58), rubiks_scene.light);
    auto back_mirror = std::make_shared<MirrorShader>(Color(0.95, 0.95, 0.98));

    add_rubiks_cube(rubiks_scene, black);

    // Matte floor, just under the cube so the bottom face does not z-fight.
    const float floor_y = -0.02f;
    rubiks_scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-20, floor_y, 20), Point3(20, floor_y, 20), Point3(20, floor_y, -20), floor));
    rubiks_scene.objects.push_back(std::make_shared<Triangle>(
        Point3(-20, floor_y, 20), Point3(20, floor_y, -20), Point3(-20, floor_y, -20), floor));

    // Mirror opposite the camera. The camera looks down along x and z together,
    // so this wall faces back toward it instead of sitting parallel to the yellow face.
    const Point3 wall(-10, 0, 5.5);
    const Vec3 wall_u(12, 0, -12);
    const Vec3 wall_v(0, 8, 0);
    rubiks_scene.objects.push_back(std::make_shared<Triangle>(
        wall, wall + wall_u, wall + wall_u + wall_v, back_mirror));
    rubiks_scene.objects.push_back(std::make_shared<Triangle>(
        wall, wall + wall_u + wall_v, wall + wall_v, back_mirror));

    build_bvh(rubiks_scene.objects);

    rubiks_cam.render(rubiks_scene, "rubiks.png");

    render_test_scene(image_width, aspect_ratio, rpp_set ? rpp : 100);
}
