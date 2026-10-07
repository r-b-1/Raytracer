#include "rubiks_cube.h"

#include "blinn_phong_shader.h"
#include "triangle.h"

// The edge order makes both triangles point in the same direction.
static void add_square(Scene& scene, const point3& origin, const vec3& u, const vec3& v,
                const std::shared_ptr<Shader>& shader) {
    const point3 b = origin + u;
    const point3 c = origin + u + v;
    const point3 d = origin + v;
    scene.objects.push_back(std::make_shared<triangle>(origin, b, c, shader));
    scene.objects.push_back(std::make_shared<triangle>(origin, c, d, shader));
}

void add_rubiks_cube(Scene& scene, const std::shared_ptr<Shader>& base_shader) {
    struct Face {
        point3 origin;
        vec3 u, v; // Unit directions across the face; cross(u, v) points outward.
        color tile_color;
        PointLight light;
        float specular_strength = 1.0f;
    };

    constexpr double face_size = 3.0;
    constexpr double tile_size = 0.7;
    constexpr double tile_step = 1.0;
    constexpr double margin = 0.15;
    constexpr double tile_offset = 0.01;

    // Only the three faces visible from (3, 5, 3) get dedicated highlight lights.
    // The other faces use the scene light, like the black base.
    const Face faces[] = {
        // Bottom
        {point3(-1.5, 0, -1.5), vec3(1, 0, 0), vec3(0, 0, 1),
         color(0.5, 0, 0.5), scene.light},
        // Back
        {point3(-1.5, 0, -1.5), vec3(0, 1, 0), vec3(1, 0, 0),
         color(0.5, 0.5, 0), scene.light},
        // Left
        {point3(-1.5, 0, -1.5), vec3(0, 0, 1), vec3(0, 1, 0),
         color(0, 0.5, 0.5), scene.light},
        // Right
        {point3(1.5, 0, -1.5), vec3(0, 1, 0), vec3(0, 0, 1),
         color(1, 0.5, 0.5), PointLight(point3(4, -2, -3))},
        // Front
        {point3(-1.5, 0, 1.5), vec3(1, 0, 0), vec3(0, 1, 0),
         color(0.5, 0.5, 1), PointLight(point3(-3, -2, 4))},
        // Top
        {point3(-1.5, 3, -1.5), vec3(0, 0, 1), vec3(1, 0, 0),
         color(0.5, 1, 0.5), PointLight(point3(-3, 5, -3)), 1.01f},
    };

    for (const auto& face : faces) {
        add_square(scene, face.origin, face_size * face.u, face_size * face.v, base_shader);
        auto tiles = std::make_shared<BlinnPhongShader>(
            face.tile_color, color(1, 1, 1), 64.0f, face.specular_strength,
            face.light);
        const vec3 outward = cross(face.u, face.v);

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                const point3 origin = face.origin
                    + (margin + col * tile_step) * face.u
                    + (margin + row * tile_step) * face.v
                    + tile_offset * outward;
                add_square(scene, origin, tile_size * face.u, tile_size * face.v, tiles);
            }
        }
    }
}
