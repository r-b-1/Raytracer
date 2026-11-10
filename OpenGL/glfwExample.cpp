#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL.h" // Open Graphics Library
#include "orthographic_camera.h"
#include "perspective_camera.h"

int CheckGLErrors(const char *s)
{
    int errCount = 0;
    return errCount;
}

int main(int argc, char* argv[]) // Host = CPU, Device = GPU
{
    // Resolve shader files relative to the current directory first, then the
    // directory containing this executable, so the program can be launched
    // from anywhere (e.g. "./buildVCPkg/OpenGL/glfwExample").
    namespace fs = std::filesystem;
    fs::path exeDir = (argc > 0) ? fs::absolute(argv[0]).parent_path() : fs::path();
    auto resolveShaderPath = [&](const char* file) -> std::string {
        if (fs::exists(file)) return std::string(file);              // cwd
        fs::path nearExe = exeDir / file;
        if (fs::exists(nearExe)) return nearExe.string();            // next to the binary
        return std::string(file); // let GLSLObject produce its usual error
    };

    /* Initialize the library */
    if (!glfwInit()) {
        exit (-1);
    }
    // throw std::runtime_error("Error! initialization of glfw failed!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    int winWidth = 1000;
    float aspectRatio = 1.0; // 16.0 / 9.0; // winWidth / (float)winHeight;
    int winHeight = winWidth / aspectRatio;
    
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "GLFW Example", NULL, NULL);
    if (!window) {
        std::cerr << "GLFW did not create a window!" << std::endl;
        
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err=glewInit();
    if(err != GLEW_OK) {
        std::cerr <<"GLEW Error! glewInit failed, exiting."<< std::endl;
        exit(EXIT_FAILURE);
    }

    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Projection data comes from the same camera classes the ray tracer uses.
    // The orthographic box is 15 x 15 to match the lab; the perspective camera
    // has an image plane 2 tall at focal length 2 (about a 53 degree vertical FOV).
    OrthographicCamera orthoCam;
    orthoCam.aspect_ratio = aspectRatio;
    orthoCam.image_plane_height = 15.0f;

    PerspectiveCamera perspCam;
    perspCam.aspect_ratio = aspectRatio;
    perspCam.image_plane_height = 2.0f;
    perspCam.focal_length = 2.0f;

    // Positive distances in front of the camera; glm::perspective requires near > 0.
    const float near = 0.1f;
    const float far = 100.0f;

    Camera* activeCamera = &perspCam;

    GLuint m_triangleVBO[1];
    GLuint m_VAO;

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;

    // create a Vertex Array Buffer to hold our triangle data                                               
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    // this is the actual triangle data that will be copied to                                              
    // the GPU memory                                                                                       
    std::vector< float > host_VertexBuffer{ -3.0f, -3.0f, 0.0f,         1.0f, 0.0f, 0.0f,    // V0                                    
                                            3.0f, -3.0f, 0.0f,         0.0f, 1.0f, 0.0f,    // V1                                    
                                            0.0f, 3.0f, 0.0f,         0.0f, 0.0f, 1.0f    };   // V2                                    

    int numBytes = host_VertexBuffer.size() * sizeof(float);

    // copy the numBytes from host_VertexBuffer t the GPU and store in                                      
    // the currently bound VBO                                                                              
    glBufferData(GL_ARRAY_BUFFER, numBytes, host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // once copied, we no longer need the data on the host                                                  
    host_VertexBuffer.clear();

    // create a vertex array object that will map the attributes in                                         
    // our vertex buffer to different location attributes for our                                           
    // shaders                                                                                              
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // VAO details here - we only have 1 attribute or location                                              
    // (Position of the vertex)                                                                             
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        reinterpret_cast<void*>(3 * sizeof(GLfloat))
    );

    glBindVertexArray(0);

    // Create a shader using my GLSLObject class
    sivelab::GLSLObject shader;
    shader.addShader( resolveShaderPath("vertexShader_withMatrixTransformation.glsl").c_str(), sivelab::GLSLObject::VERTEX_SHADER );
    shader.addShader( resolveShaderPath("fragmentShader_passthrough.glsl").c_str(), sivelab::GLSLObject::FRAGMENT_SHADER );
    shader.createProgram();

    GLint projMatrixID = shader.createUniform("projMatrix");
    GLint viewMatrixID = shader.createUniform("viewMatrix");
    GLint modelMatrixID = shader.createUniform("modelMatrix");

    // Model transform state, changed every frame (spin) and by the keyboard.
    glm::vec3 modelTranslation(0.0f, 0.0f, 0.0f);
    float modelScale = 1.0f;
    float modelAngle = 0.0f;
    bool spinning = true;
    bool spinKeyWasDown = false;
    const float spinRatePerSecond = glm::radians(45.0f);

    glm::vec3 m_pos(0,0,10);
    glm::vec3 m_U(1,0,0), m_V(0,1,0), m_W(0,0,1);

    // Camera look direction is derived from the basis: forward = -m_W.
    // (Pitch/yaw rotation disabled for now — ortho camera only needs WASD.)
    // const glm::vec3 upWorld(0.0f, 1.0f, 0.0f); // fixed axis we yaw around
    // float pitchAngle = 0.0f; // accumulated look-up/down angle
    // const float rotateRatePerFrame = glm::radians(1.0f);
    // const float pitchClamp = glm::radians(85.0f); // don't flip over the top

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        endFrameTime = glfwGetTime();
        timeDiff = endFrameTime - startFrameTime;
        startFrameTime = glfwGetTime();

        // Clear the window's buffer (or clear the screen to our
        // background color)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render your objects here */
        activeCamera->lookfrom = Point3(m_pos.x, m_pos.y, m_pos.z);
        glm::vec3 target = m_pos - m_W;
        activeCamera->lookat = Point3(target.x, target.y, target.z);
        activeCamera->vup = Vec3(m_V.x, m_V.y, m_V.z);

        glm::mat4 M_view = activeCamera->viewMatrix();
        glm::mat4 M_proj = activeCamera->projectionMatrix(near, far);

        // if (spinning) {
        //     modelAngle += spinRatePerSecond * static_cast<float>(timeDiff);
        // }
        // Scale, then rotate about the triangle's own Y axis, then translate.
        glm::mat4 M_model = glm::translate(glm::mat4(1.0f), modelTranslation);
        M_model = glm::rotate(M_model, modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        M_model = glm::scale(M_model, glm::vec3(modelScale));

        shader.activate();

        // copy from the host to the device the projection, view, and model matrices
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr( M_proj ));
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr( M_model ));

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        /* Poll for and process events */
        glfwPollEvents();

        float moveRatePerFrame = 0.05;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_pos = m_pos + -m_W * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_pos = m_pos - m_U * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_pos = m_pos + m_W * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_pos = m_pos + m_U * moveRatePerFrame;
        }

        // How to rotate the camera view: rotate the camera basis vectors.
        // (Disabled for now — only WASD movement is needed.)
        //
        //   F / H — yaw: rotate the basis around the world up axis
        //   T / G — pitch: rotate V (up) and W (back) around U (right)
        // if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        //     glm::mat4 R_yaw = glm::rotate(glm::mat4(1.0f), rotateRatePerFrame, upWorld);
        //     m_U = glm::vec3(R_yaw * glm::vec4(m_U, 0.0f));
        //     m_V = glm::vec3(R_yaw * glm::vec4(m_V, 0.0f));
        //     m_W = glm::vec3(R_yaw * glm::vec4(m_W, 0.0f));
        // }
        // else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        //     glm::mat4 R_yaw = glm::rotate(glm::mat4(1.0f), -rotateRatePerFrame, upWorld);
        //     m_U = glm::vec3(R_yaw * glm::vec4(m_U, 0.0f));
        //     m_V = glm::vec3(R_yaw * glm::vec4(m_V, 0.0f));
        //     m_W = glm::vec3(R_yaw * glm::vec4(m_W, 0.0f));
        // }

        // if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && pitchAngle < pitchClamp) {
        //     pitchAngle += rotateRatePerFrame;
        //     glm::mat4 R_pitch = glm::rotate(glm::mat4(1.0f), rotateRatePerFrame, m_U);
        //     m_V = glm::vec3(R_pitch * glm::vec4(m_V, 0.0f));
        //     m_W = glm::vec3(R_pitch * glm::vec4(m_W, 0.0f));
        // }
        // else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && pitchAngle > -pitchClamp) {
        //     pitchAngle -= rotateRatePerFrame;
        //     glm::mat4 R_pitch = glm::rotate(glm::mat4(1.0f), -rotateRatePerFrame, m_U);
        //     m_V = glm::vec3(R_pitch * glm::vec4(m_V, 0.0f));
        //     m_W = glm::vec3(R_pitch * glm::vec4(m_W, 0.0f));
        // }

        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        std::cout << "fps: " << 1.0 / timeDiff << std::endl;
        }

        //   P / O — perspective / orthographic projection
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            activeCamera = &perspCam;
        }
        else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            activeCamera = &orthoCam;
        }

        // Model matrix controls:
        //   arrow keys — translate the triangle in X / Y
        //   Z / X      — shrink / grow the triangle
        //   R          — toggle spinning
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            modelTranslation.x -= moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            modelTranslation.x += moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            modelTranslation.y += moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            modelTranslation.y -= moveRatePerFrame;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            modelScale = glm::max(0.1f, modelScale - 0.01f);
        }
        else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            modelScale += 0.01f;
        }

        bool spinKeyDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
        if (spinKeyDown && !spinKeyWasDown) {
            spinning = !spinning;
        }
        spinKeyWasDown = spinKeyDown;

        shader.deactivate();

        // Swap the front and back buffers
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }
  
    glfwTerminate();
    return 0;
}
