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

configure_file(
    vertexShader_passthrough.glsl
    ${CMAKE_BINARY_DIR}/vertexShader_passthrough.glsl
    COPYONLY
)

configure_file(
    vertexShader_withMatrixTransformation.glsl
    ${CMAKE_BINARY_DIR}/vertexShader_withMatrixTransformation.glsl
    COPYONLY
)

configure_file(
    fragmentShader_passthrough.glsl
    ${CMAKE_BINARY_DIR}/fragmentShader_passthrough.glsl
    COPYONLY
)

# Also copy the shaders next to the executable (configure_file() can't use
# generator expressions like $<TARGET_FILE_DIR:...>, so this runs post-build).
# This lets OpenGL/glfwExample be run from anywhere, including the project root.
add_custom_command(TARGET glfwExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/vertexShader_passthrough.glsl
        $<TARGET_FILE_DIR:glfwExample>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/vertexShader_withMatrixTransformation.glsl
        $<TARGET_FILE_DIR:glfwExample>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/fragmentShader_passthrough.glsl
        $<TARGET_FILE_DIR:glfwExample>
    COMMENT "Copying GLSL shaders next to glfwExample"
)

