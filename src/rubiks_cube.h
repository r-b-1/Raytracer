#pragma once

#include <memory>

#include "scene.h"

// Add a cube with six 3x3 tiled faces, spanning (-1.5, 0, -1.5) to (1.5, 3, 1.5).
// The base uses the supplied shader; each face has its own shiny tile material.
void add_rubiks_cube(Scene& scene, const std::shared_ptr<Shader>& base_shader);
