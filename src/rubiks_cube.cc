#include "rubiks_cube.h"

#include "blinn_phong_shader.h"
#include "triangle.h"

// The edge order makes both triangles point in the same direction.
static void add_square(Scene& scene, const Point3& origin, const Vec3& u, const Vec3& v,
                const std::shared_ptr<Shader>& shader) {
    const Point3 b = origin + u;
    const Point3 c = origin + u + v;
    const Point3 d = origin + v;
    scene.objects.push_back(std::make_shared<Triangle>(origin, b, c, shader));
    scene.objects.push_back(std::make_shared<Triangle>(origin, c, d, shader));
}

void add_rubiks_cube(Scene& scene, const std::shared_ptr<Shader>& base_shader) {
    struct Face {
        Point3 origin;
        Vec3 u, v; // Unit directions across the face; cross(u, v) points outward.
        Color tile_color;
        PointLight light;
        float specular_strength = 1.0f;
    };

    constexpr float face_size = 3.0;
    constexpr float tile_size = 0.7;
    constexpr float tile_step = 1.0;
    constexpr float margin = 0.15;
    constexpr float tile_offset = 0.01;

    // Only the three faces visible from (3, 5, 3) get dedicated highlight lights.
    // The other faces use the scene light, like the black base.
    const Face faces[] = {
        // Bottom
        {Point3(-1.5, 0, -1.5), Vec3(1, 0, 0), Vec3(0, 0, 1),
         Color(0.5, 0, 0.5), scene.light},
        // Back
        {Point3(-1.5, 0, -1.5), Vec3(0, 1, 0), Vec3(1, 0, 0),
         Color(0.5, 0.5, 0), scene.light},
        // Left
        {Point3(-1.5, 0, -1.5), Vec3(0, 0, 1), Vec3(0, 1, 0),
         Color(0, 0.5, 0.5), scene.light},
        // Right
        {Point3(1.5, 0, -1.5), Vec3(0, 1, 0), Vec3(0, 0, 1),
         Color(1, 0.5, 0.5), PointLight(Point3(4, -2, -3))},
        // Front
        {Point3(-1.5, 0, 1.5), Vec3(1, 0, 0), Vec3(0, 1, 0),
         Color(0.5, 0.5, 1), PointLight(Point3(-3, -2, 4))},
        // Top
        {Point3(-1.5, 3, -1.5), Vec3(0, 0, 1), Vec3(1, 0, 0),
         Color(0.5, 1, 0.5), PointLight(Point3(-3, 5, -3)), 1.01f},
    };

    for (const auto& face : faces) {
        add_square(scene, face.origin, face_size * face.u, face_size * face.v, base_shader);
        auto tiles = std::make_shared<BlinnPhongShader>(
            face.tile_color, Color(1, 1, 1), 64.0f, face.specular_strength,
            face.light);
        const Vec3 outward = cross(face.u, face.v);

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                const Point3 origin = face.origin
                    + (margin + col * tile_step) * face.u
                    + (margin + row * tile_step) * face.v
                    + tile_offset * outward;
                add_square(scene, origin, tile_size * face.u, tile_size * face.v, tiles);
            }
        }
    }
}
