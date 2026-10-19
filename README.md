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

## Tests

```
./buildVCPkg/utests/utest_Success
```
