#include "rubiks_cube.h"

#include "mirror_shader.h"
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
    };

    constexpr float face_size = 3.0;
    constexpr float tile_size = 0.7;
    constexpr float tile_step = 1.0;
    constexpr float margin = 0.15;
    constexpr float tile_offset = 0.01;

    // Each sticker is a mirror tinted with that face's color. From (3, 5, 3)
    // the top, front, and right faces are the ones turned toward the camera.
    const Face faces[] = {
        // Bottom
        {Point3(-1.5, 0, -1.5), Vec3(1, 0, 0), Vec3(0, 0, 1), Color(0.55, 0.1, 0.7)},
        // Back
        {Point3(-1.5, 0, -1.5), Vec3(0, 1, 0), Vec3(1, 0, 0), Color(0.95, 0.85, 0.15)},
        // Left
        {Point3(-1.5, 0, -1.5), Vec3(0, 0, 1), Vec3(0, 1, 0), Color(0.1, 0.75, 0.35)},
        // Right
        {Point3(1.5, 0, -1.5), Vec3(0, 1, 0), Vec3(0, 0, 1), Color(1.0, 0.12, 0.08)},
        // Front
        {Point3(-1.5, 0, 1.5), Vec3(1, 0, 0), Vec3(0, 1, 0), Color(0.1, 0.25, 1.0)},
        // Top
        {Point3(-1.5, 3, -1.5), Vec3(0, 0, 1), Vec3(1, 0, 0), Color(1.0, 0.95, 0.9)},
    };

    for (const auto& face : faces) {
        add_square(scene, face.origin, face_size * face.u, face_size * face.v, base_shader);
        auto tiles = std::make_shared<MirrorShader>(face.tile_color);
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
