# Ray Tracer

A ray tracer that renders spheres and triangles with Lambertian, Blinn-Phong, and mirror materials. It writes PNG images through a framebuffer, and the image size comes from the command line.

## Build

This project uses CMake and [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview). `VCPKG_ROOT` must point at a bootstrapped vcpkg install.

On macOS, the renderer also needs the OpenMP runtime:

```
brew install cmake libomp
```

From the project root:

```
cmake --preset=default
cmake --build buildVCPkg
```

## Run

```
./buildVCPkg/main --width [width] --height [height]
```

With no size flags the images are 1000×1000. Passing only `--width` or only `--height` keeps the image square. `--aspect` overrides that ratio.

Running the program renders three scenes. The test scene uses every shader and is drawn with both the orthographic and the perspective camera. The snowman is an original scene. The Rubik's cube has colored mirror tiles. The three faces turned toward the camera are the ones that are lit, and a mirror stands behind the cube.

## OpenGL controls

Controls for the interactive OpenGL rasterizer (`buildVCPkg/OpenGL/glfwExample`):

| Key | Action |
|---|---|
| `W` / `S` | move camera forward / backward along its view direction |
| `A` / `D` | move camera left / right along its right vector |
| `Left` / `Right` | translate the bunny along world X |
| `Up` / `Down` | translate the bunny along world Y |
| `Z` / `X` | shrink / grow the bunny |
| `R` | toggle the bunny's continuous spin about Y |
| `1` / `2` / `3` | Lambertian / Blinn-Phong / Normal shading |
| `P` / `O` | perspective / orthographic projection |
| `Y` | print the current frames per second |
| `Esc` | quit |

## Side-by-side comparison

The same scene rendered two ways: `raytrace.png` (ray tracer) and `rasterizer.png` (OpenGL).

| Ray tracer | OpenGL rasterizer |
|---|---|
| ![Ray tracer](raytrace.png) | ![OpenGL](rasterizer.png) |

Both use the same geometry (bunny + two snowmen built from spheres + a sun sphere in the background), the same point light at `(0, 6, 5)`, the same camera at `(0, 0, 10)` with a 53° vertical field of view, and the same Blinn-Phong parameters (exponent 32, white specular). The ray tracer image uses 16 jittered samples per pixel; the OpenGL image is a 1000×1000 capture of the window in Blinn-Phong mode.

Visual differences:

- **Shadows.** The ray tracer casts shadow rays, so the bunny and the snowmen have contact shadows on the floor. The rasterizer has no shadow mapping, so the floor is uniformly lit and the objects appear to float slightly. This is the most obvious difference.
- **Bunny shading.** The rasterizer uses the smooth per-vertex normals that the OBJ loader generates, so the bunny looks curved. The ray tracer's triangle uses the geometric face normal, so the same mesh is visibly faceted there.
- **Background.** The ray tracer has a sky gradient based on the ray direction; the rasterizer clears to black.
- **Antialiasing.** The ray tracer averages 16 jittered samples per pixel, so silhouettes and highlights are smooth. The rasterizer renders one sample per pixel with no MSAA, so edges are aliased.
- **Specular highlights.** Both use Blinn-Phong, but the ray tracer evaluates it exactly at each hit, while the rasterizer interpolates the per-vertex normals and light directions and shades per fragment. The highlights land in the same places but are slightly softer in the rasterizer.
- **Floor material.** The ray tracer uses a Lambertian floor; the rasterizer uses the active Blinn-Phong program with the specular term set to zero for the floor, so both read as diffuse.

## Tests

```
./buildVCPkg/utests/utest_Success
```
