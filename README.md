# Ray Tracer — BVH Bronze Extension

A C++20 ray tracer for spheres and triangles, with Lambertian, Blinn-Phong, and mirror materials, shadow rays, and perspective and orthographic cameras. Renders are saved as PNG files.

## Build

Install CMake, a C++20 compiler, and a bootstrapped [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview). Set `VCPKG_ROOT` to that vcpkg installation. CMake installs the dependencies listed in `vcpkg.json` and downloads Catch2. On macOS, install the OpenMP runtime with `brew install cmake libomp`.

From the repository root, build with optimizations enabled for meaningful timings:

```sh
cmake --preset=default -DCMAKE_BUILD_TYPE=Release
cmake --build buildVCPkg --target main utest_Success -j 8
```

## Render and verify

```sh
./buildVCPkg/utests/utest_Success
./buildVCPkg/main --width 1000 --height 1000 --rpp 10
```

The renderer uses the BVH by default. Add `--no-bvh` to use a flat object list. Both modes retain the same materials, lights, shadows, reflection depths, and camera samples. The flat baseline still checks each object's bounding box; it has no hierarchical pruning.

Each run writes four images in the working directory:

- `snowman.png`: 11 spheres, including the body, eyes, buttons, nose, ground, and sun; a perspective camera with diffuse and specular shading.
- `rubiks.png`: a tiled Rubik's cube with colored mirror tiles, a floor, and a mirror wall. This scene contains 124 triangles and uses up to five reflection bounces.
- `scene_perspective.png`: 9 spheres and 5 triangles, including a floor and mirrored wall, with two lights and diffuse, specular, normal, and mirror materials; five reflection bounces.
- `scene_orthographic.png`: the same mixed scene through an orthographic camera; five reflection bounces.

The default image size is 1000×1000. One size flag keeps the image square; `--aspect` overrides the width/height ratio. Specify `--rpp` to set samples per pixel for all scenes. Without it, snowman and cube use 10 samples and the mixed scene uses 100.

The tests cover vector/ray operations, parallel and grazing box intersections, empty and single-object trees, and nearest-hit equivalence with direct primitive intersections for thousands of rays through mixed geometry. The starter repository also contains `utest_Failure`, an intentionally failing demonstration; run `utest_Success` for the verification suite.

## BVH implementation

`src/aabb.h` implements axis-aligned boxes and slab intersection. Parallel rays are checked explicitly, grazing intersections are retained, and triangle bounds are padded to give flat faces thickness. `src/bvh.h` builds a binary hierarchy by sorting object bounds along the widest axis and splitting the list at its midpoint. Leaves contain one or two primitives. Traversal skips a subtree when its box is missed and tightens the maximum hit distance as nearer intersections are found. Empty trees are safe, and singleton leaves intersect their primitive once.

Camera jitter uses a fixed per-row random seed, so flat and BVH renders receive identical rays even when OpenMP schedules rows differently.

## Reproduce the timings

Python 3 is used only to automate repeated renders and compare output files:

```sh
python3 benchmarks/run.py --threads 1 --repeats 3
```

This renders all four views three times per mode at **1000×1000, 10 rays per pixel** (10 million primary rays per image, plus shadow and reflection rays). It writes images, progress logs, and `timings.csv` under `buildVCPkg/benchmark/`, verifies that each flat/BVH PNG pair is byte-identical, and prints median render times and speedups. `--executable` and `--output` can select another binary or output directory.

To run one comparison manually in the current directory:

```sh
OMP_NUM_THREADS=1 ./buildVCPkg/main --benchmark --width 1000 --height 1000 --rpp 10 > timings.csv
```

Benchmark images use `_flat.png` and `_bvh.png` suffixes. CSV `render_seconds` measures camera initialization, ray tracing, and PNG export with a steady wall clock. `build_seconds` measures BVH construction separately; scene creation and the shared-pointer list copy are excluded. Flat mode is measured before BVH mode in each pair. Timings depend on hardware and background activity; one OpenMP thread is used to isolate the hierarchy's benefit.

Measured results and raw timing data are in [benchmarks/results.md](benchmarks/results.md) and [benchmarks/timings.csv](benchmarks/timings.csv).
