add_library (cs4212-util
  ArgumentParsing.cpp ArgumentParsing.h
  handleGraphicsArgs.cpp handleGraphicsArgs.h
  model_obj.cpp model_obj.h
  ISceneLoader.h SceneLoader.h
  SceneParser_JSON.cpp SceneParser_JSON.h
  camera.h
  perspective_camera.h
  orthographic_camera.h
  color.h
  vec3.h
  ray.h
  shape.h
  sphere.h
  triangle.h
  scene.h
  framebuffer.h
  framebuffer.cc
  triangle.h
  point_light.h
  shader.h
  normal_shader.h
  normal_shader.cc
  lambertian_shader.h
  lambertian_shader.cc
  blinn_phong_shader.h
  blinn_phong_shader.cc
  rubiks_cube.h
  rubiks_cube.cc
  mirror_shader.cc 
  mirror_shader.h
  aabb.h
  bvh.h
)
target_compile_definitions(cs4212-util PUBLIC HAS_GLM)
target_link_libraries(cs4212-util PRIVATE Boost::program_options)
target_link_libraries(cs4212-util PUBLIC glm::glm)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(cs4212-util PUBLIC nlohmann_json::nlohmann_json)

# camera.h carries a `#pragma omp parallel for`, so every translation unit that
# includes it needs both the OpenMP compile flag and the runtime.
if(OpenMP_CXX_FOUND)
  target_link_libraries(cs4212-util PUBLIC OpenMP::OpenMP_CXX)
endif()
