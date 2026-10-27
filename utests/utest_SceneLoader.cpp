#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "SceneLoader.h"
#include "SceneParser_JSON.h"

TEST_CASE("JSON cameras and point lights populate the renderer scene") {
    Scene scene;
    auto loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);
    parser.parseStringData(R"({"scene": {
        "camera": [
            {"type": "perspective", "position": [1, 2, 3],
             "viewDir": [0, 0, -2], "focalLength": 0.4, "imagePlaneWidth": 0.5},
            {"type": "orthographic", "position": "0 3 3",
             "lookatPoint": [0, 2, 0], "imagePlaneWidth": 5}
        ],
        "light": [
            {"type": "point", "position": [-5, 8, 5], "intensity": [0.7, 0.6, 0.45]},
            {"type": "point", "position": [6, 6, 2], "intensity": [0.35, 0.45, 0.7]}
        ]
    }})");

    REQUIRE(scene.cameras.size() == 2);
    REQUIRE(std::dynamic_pointer_cast<PerspectiveCamera>(scene.cameras[0]));
    REQUIRE(std::dynamic_pointer_cast<OrthographicCamera>(scene.cameras[1]));
    REQUIRE(scene.cameras[0]->lookfrom.x() == 1);
    REQUIRE(scene.cameras[0]->lookat.z() == 1);
    REQUIRE(scene.cameras[0]->focal_length == 0.4f);
    REQUIRE(scene.cameras[0]->image_plane_width == 0.5f);
    REQUIRE(scene.cameras[1]->lookat.y() == 2);
    REQUIRE(scene.cameras[1]->lookat.z() == 0);
    REQUIRE(scene.lights.size() == 2);
    REQUIRE(scene.lights[0].position().x() == -5);
    REQUIRE(scene.lights[0].intensity().x() == 0.7f);
    REQUIRE(scene.lights[1].position().x() == 6);
    REQUIRE(scene.lights[1].intensity().z() == 0.7f);
}

TEST_CASE("JSON mesh paths are relative to the scene file") {
    Scene scene;
    auto loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);
    parser.parseFileData(std::string(SCENE_TEST_DIR) + "/fixtures/mesh.json");
    REQUIRE(scene.objects.size() == 1);
    REQUIRE(std::dynamic_pointer_cast<Triangle>(scene.objects[0]));
    REQUIRE(std::dynamic_pointer_cast<LambertianShader>(scene.objects[0]->shader()));
    float tmax = 10;
    HitStruct hit;
    REQUIRE(scene.intersect(Ray(Point3(0.2, 0.2, 1), Vec3(0, 0, -1)), 0.001f, tmax, hit));
    REQUIRE(hit.t() == 1);
}

TEST_CASE("JSON shapes use named materials and all parsed lights") {
    Scene scene;
    auto loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);
    parser.parseStringData(R"({
        "sceneParameters": {"bgColor": [0.1, 0.2, 0.3]},
        "light": [
            {"position": [0, 0, 3], "intensity": [1, 0, 0]},
            {"position": [0, 0, 4], "intensity": [0, 1, 0]}
        ],
        "shader": [
            {"type": "Lambertian", "name": "matte", "diffuse": [0.5, 0.5, 0.5]},
            {"type": "BlinnPhong", "name": "shiny", "diffuse": [1, 0, 0],
             "specular": [1, 1, 1], "phongExp": 64},
            {"type": "Mirror", "name": "mirror"},
            {"type": "Normal", "name": "normals"}
        ],
        "shape": [
            {"type": "sphere", "center": [0, 0, 0], "radius": 1,
             "shader": {"ref": "matte"}},
            {"type": "triangle", "v0": [3, 0, 0], "v1": [4, 0, 0],
             "v2": [3, 1, 0], "shader": {"ref": "shiny"}},
            {"type": "sphere", "center": [6, 0, 0], "shader": {"ref": "mirror"}},
            {"type": "sphere", "center": [9, 0, 0], "shader": {"ref": "normals"}}
        ]
    })");

    REQUIRE(scene.objects.size() == 4);
    REQUIRE(std::dynamic_pointer_cast<Sphere>(scene.objects[0]));
    REQUIRE(std::dynamic_pointer_cast<Triangle>(scene.objects[1]));
    REQUIRE(std::dynamic_pointer_cast<LambertianShader>(scene.objects[0]->shader()));
    auto shiny = std::dynamic_pointer_cast<BlinnPhongShader>(scene.objects[1]->shader());
    REQUIRE(shiny);
    REQUIRE(shiny->shininess() == 64);
    REQUIRE(std::dynamic_pointer_cast<MirrorShader>(scene.objects[2]->shader()));
    REQUIRE(std::dynamic_pointer_cast<NormalShader>(scene.objects[3]->shader()));

    const Color lit = trace(scene, Ray(Point3(0, 0, 3), Vec3(0, 0, -1)), 2);
    REQUIRE(lit.x() == 0.5f);
    REQUIRE(lit.y() == 0.5f);
    REQUIRE(lit.z() == 0.0f);
    const Color background = trace(scene, Ray(Point3(0, 0, 3), Vec3(0, 1, 0)), 2);
    REQUIRE(background.x() == 0.1f);
    REQUIRE(background.y() == 0.2f);
    REQUIRE(background.z() == 0.3f);
}

TEST_CASE("Unsupported scene features and missing materials report errors") {
    Scene scene;
    auto loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);
    REQUIRE_THROWS_WITH(parser.parseStringData(R"({"camera": [
        {"type": "fisheye", "position": [0,0,0], "viewDir": [0,0,-1]}
    ]})"), "Unsupported camera type: fisheye");
    REQUIRE_THROWS_WITH(parser.parseStringData(R"({"shape": [
        {"type": "sphere", "shader": {"ref": "missing"}}
    ]})"), "Unknown shader reference: missing");
    REQUIRE_THROWS_WITH(parser.parseStringData(R"({"shape": [
        {"type": "sphere", "xform": [{"type": "translate", "amount": [1,0,0]}]}
    ]})"), "Shape transforms are not supported by SceneLoader");
}
