add_library (cs4212-util
  ArgumentParsing.cpp ArgumentParsing.h
  handleGraphicsArgs.cpp handleGraphicsArgs.h
  model_obj.cpp model_obj.h
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
  interval.h
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
)
target_compile_definitions(cs4212-util PUBLIC HAS_GLM)
target_link_libraries(cs4212-util PRIVATE Boost::program_options)
target_link_libraries(cs4212-util PUBLIC glm::glm)
