include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable (glfwExample
  glfwExample.cpp 
  GLSL.cpp GLSL.h
)

target_link_libraries (glfwExample PRIVATE GLEW::GLEW)
target_link_libraries (glfwExample PRIVATE glfw)

if(APPLE)
    target_link_libraries(glfwExample PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(glfwExample PRIVATE OpenGL::GL)
endif()

target_link_libraries (glfwExample PRIVATE PNG::PNG)
target_link_libraries (glfwExample PRIVATE ZLIB::ZLIB)

configure_file(
    vertexShader_passthrough.glsl
    ${CMAKE_BINARY_DIR}/vertexShader_passthrough.glsl
    COPYONLY
)

configure_file(
    fragmentShader_passthrough.glsl
    ${CMAKE_BINARY_DIR}/fragmentShader_passthrough.glsl
    COPYONLY
)

