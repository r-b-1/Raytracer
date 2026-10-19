#include "normal_shader.h"

#include "shape.h"

Color NormalShader::rayColor(const HitStruct& h, const Scene&, int) const {
    // Map each normal component from [-1, 1] to [0, 1].
    return 0.5f * (h.normal() + Color(1, 1, 1));
}
