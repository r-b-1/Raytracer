#pragma once

#include <memory>

#include "scene.h"

// Add a cube with six 3x3 tiled faces, spanning (-1.5, 0, -1.5) to (1.5, 3, 1.5).
// Top, front, and right tiles have highlight lights for the main camera view.
// The other tiles use scene.light; the base uses the supplied shader.
void add_rubiks_cube(Scene& scene, const std::shared_ptr<Shader>& base_shader);
