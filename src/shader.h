#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <memory>

#include "color.h"
#include "point_light.h"
#include "vec3.h"

// Defined in shape.h / scene.h; forward-declared to avoid a circular include.
class HitStruct;
class Scene;

class Shader {
public:
    virtual ~Shader() = default;


    virtual Color rayColor(const HitStruct& h, const Scene& scene, int depth) const = 0;


    const std::vector<std::shared_ptr<Light>>& lights() const { return lights_; }

protected:
    explicit Shader(const PointLight& light = PointLight())
        : lights_{std::make_shared<PointLight>(light)} {}
    explicit Shader(const std::vector<std::shared_ptr<Light>>& lights) : lights_(lights) {}

    std::vector<std::shared_ptr<Light>> lights_;
};

#endif
