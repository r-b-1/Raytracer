#ifndef SHADER_H
#define SHADER_H

#include <vector>

#include "color.h"
#include "point_light.h"
#include "vec3.h"

// Defined in shape.h / scene.h; forward-declared to avoid a circular include.
class HitStruct;
class Scene;

class Shader {
public:
    virtual ~Shader() = default;

    // `scene` lets a material trace secondary rays out of the surface (see
    // MirrorShader). `depth` is the remaining bounce budget: a material that
    // traces a secondary ray decrements it, and trace() returns black once it
    // runs out. Materials that shade directly can ignore both.
    virtual Color rayColor(const HitStruct& h, const Scene& scene, int depth) const = 0;

    // Every light this material responds to. A material may carry its own list
    // (a scene lit from two sides, or the per-face highlights on the rubiks
    // cube); materials that only need the ambient default get one white light.
    const std::vector<PointLight>& lights() const { return lights_; }

protected:
    explicit Shader(const PointLight& light = PointLight()) : lights_{light} {}
    explicit Shader(const std::vector<PointLight>& lights) : lights_(lights) {}

    std::vector<PointLight> lights_;
};

#endif