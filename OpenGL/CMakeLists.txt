include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable (glfwExample
  glfwExample.cpp 
  GLSL.cpp GLSL.h
)

target_link_libraries (glfwExample PRIVATE cs4212-util)
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

# Every shader is copied into the build tree and next to the executable so the
# program can be launched from anywhere (including the project root).
set(GLSL_SHADERS
    vertexShader_passthrough.glsl
    vertexShader_withMatrixTransformation.glsl
    vertexShader_PrepForPerFragment.glsl
    fragmentShader_Lambertian.glsl
    fragmentShader_BlinnPhong.glsl
    fragmentShader_Normal.glsl
)

foreach(shader ${GLSL_SHADERS})
    configure_file(
        ${shader}
        ${CMAKE_BINARY_DIR}/${shader}
        COPYONLY
    )

    # configure_file() can't use generator expressions like
    # $<TARGET_FILE_DIR:...>, so the copy next to the binary runs post-build.
    add_custom_command(TARGET glfwExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${shader}
            $<TARGET_FILE_DIR:glfwExample>
    )
endforeach()


