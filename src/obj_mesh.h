#ifndef OBJ_MESH_H
#define OBJ_MESH_H

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "bvh.h"
#include "model_obj.h"
#include "triangle.h"

// Loads an OBJ file as one BVH-backed Shape made of triangles. The model is
// centered on `position` and scaled so its longest side is `size`.
inline std::shared_ptr<Shape> load_obj_mesh(const std::string& filename,
                                            std::shared_ptr<Shader> shader,
                                            const Point3& position = Point3(0, 0, 0),
                                            float size = 1.0f) {
    ModelOBJ obj;
    if (!obj.import(filename.c_str()))
        throw std::runtime_error("could not open OBJ file: " + filename);
    if (obj.getNumberOfTriangles() <= 0)
        throw std::runtime_error("OBJ file has no triangles: " + filename);

    obj.normalize(size, true);

    const ModelOBJ::Vertex* vertices = obj.getVertexBuffer();
    const int* indices = obj.getIndexBuffer();

    std::vector<std::shared_ptr<Shape>> triangles;
    triangles.reserve(static_cast<size_t>(obj.getNumberOfTriangles()));

    for (int i = 0; i < obj.getNumberOfIndices(); i += 3) {
        const auto place = [&](int index) {
            const float* p = vertices[index].position;
            return Point3(p[0] + position.x(), p[1] + position.y(), p[2] + position.z());
        };
        triangles.push_back(std::make_shared<Triangle>(
            place(indices[i]), place(indices[i + 1]), place(indices[i + 2]), shader));
    }

    return std::make_shared<BvhNode>(triangles);
}

#endif
