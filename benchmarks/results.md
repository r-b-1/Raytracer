# BVH timing results

Measured November 2, 2026 on macOS 27.0 (arm64), Apple Clang 21.0.0, CMake Release build, OpenMP with `OMP_NUM_THREADS=1`. Each image is 1000×1000 with 10 samples per pixel. Values below are medians of three runs; raw measurements are in [timings.csv](timings.csv).

| Scene | Primitives | Flat render (s) | BVH render (s) | BVH build (s) | Speedup |
| --- | ---: | ---: | ---: | ---: | ---: |
| snowman.png | 11 | 5.474 | 4.774 | 0.000007 | 1.15× |
| rubiks.png | 124 | 35.265 | 9.949 | 0.000075 | 3.54× |
| scene_perspective.png | 14 | 7.499 | 9.880 | 0.000006 | 0.76× |
| scene_orthographic.png | 14 | 8.664 | 11.698 | 0.000007 | 0.74× |

Speedup is flat render time divided by BVH render time; values below 1 mean a slowdown. Rendering includes camera initialization, ray tracing, progress logging, and PNG encoding/writing. Tree construction is reported separately and is negligible for these scenes. Geometry creation and copying the object list are excluded. Flat mode retains per-object AABB checks and runs before BVH mode in each trial. There is no separate warm-up; background activity, caches, and fixed ordering can influence timings.

The 124-triangle Rubik’s cube benefits most because whole groups of tiles can be rejected together. The 11-sphere snowman shows a modest improvement. Both views of the 14-object mixed scene are slower with this BVH: the scene is small, its floor and mirror have large bounds, and additional tree traversal costs more than the primitive tests saved. These measurements show that a BVH does not guarantee faster rendering for every scene.

All four flat/BVH PNG pairs were byte-identical in each of the three trials (12 comparisons). The cameras use identical deterministic jitter samples. The correctness suite also passed 7,406 assertions across 18 tests, including comparison against primitive intersections without AABB filtering.

Scene descriptions and build/run instructions are in the [README](../README.md). Reproduce from the repository root:

```sh
python3 benchmarks/run.py --threads 1 --repeats 3
```

Generated images and progress logs remain under `buildVCPkg/benchmark/` (ignored build output). The script fails if a render fails or paired images differ.
