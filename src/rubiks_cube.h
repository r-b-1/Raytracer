#ifndef RUBIKS_CUBE_H
#define RUBIKS_CUBE_H

#include <memory>

#include "scene.h"

// Add a cube with six 3x3 tiled faces, spanning (-1.5, 0, -1.5) to (1.5, 3, 1.5).
// Every sticker is a mirror tinted with that face's color. The base uses the
// supplied shader.
void add_rubiks_cube(Scene& scene, const std::shared_ptr<Shader>& base_shader);

#endif
