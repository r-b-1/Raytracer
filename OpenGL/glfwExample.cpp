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
#include "model_obj.h"
#include "orthographic_camera.h"
#include "perspective_camera.h"

namespace {

enum ShaderMode {
    LAMBERTIAN,
    BLINN_PHONG,
    NORMAL_SHADER,
    NumShaders
};

const char* shaderNames[NumShaders] = { "Lambertian", "Blinn-Phong", "Normal" };

// Uniform locations are per program. A location of -1 means that program does
// not use the uniform, and glUniform silently ignores -1.
struct ShaderUniforms {
    GLint projMatrixID = -1;
    GLint viewMatrixID = -1;
    GLint modelMatrixID = -1;
    GLint normalMatrixID = -1;
    GLint lightPosWorldID = -1;
    GLint diffuseComponentID = -1;
    GLint specularComponentID = -1;
    GLint phongExponentID = -1;
    GLint eyePosWorldID = -1;
};

} // namespace

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

    // Optional command line argument to start in a given shading mode, so the
    // same executable can be launched straight into one shader for screenshots:
    //   ./glfwExample blinn   |   ./glfwExample normal   |   ./glfwExample lambert
    int activeShader = LAMBERTIAN;
    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "blinn" || arg == "--blinn" || arg == "blinn-phong") {
            activeShader = BLINN_PHONG;
        }
        else if (arg == "normal" || arg == "--normal") {
            activeShader = NORMAL_SHADER;
        }
        else if (arg == "lambert" || arg == "--lambert" || arg == "lambertian") {
            activeShader = LAMBERTIAN;
        }
    }

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
    GLuint m_indexVBO;
    GLuint m_VAO;

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;

    // Load the triangle mesh: center it and scale its longest side to 6 units,
    // matching the single triangle this used to draw.
    ModelOBJ obj;
    if (!obj.import("assets/bunny.obj")) {
        std::cerr << "Could not load assets/bunny.obj (run from the project root)." << std::endl;
        glfwTerminate();
        return -1;
    }
    obj.normalize(6.0f, true);

    // Pack position (xyz) followed by normal (xyz) per vertex, the layout the
    // shaders expect.
    std::vector< float > host_VertexBuffer;
    host_VertexBuffer.reserve(obj.getNumberOfVertices() * 6);
    for (int i = 0; i < obj.getNumberOfVertices(); i++) {
        const ModelOBJ::Vertex& v = obj.getVertex(i);
        host_VertexBuffer.insert(host_VertexBuffer.end(),
                                 { v.position[0], v.position[1], v.position[2],
                                   v.normal[0], v.normal[1], v.normal[2] });
    }

    std::vector< unsigned int > indexBuffer(
        obj.getIndexBuffer(), obj.getIndexBuffer() + obj.getNumberOfIndices());

    // create a Vertex Array Buffer to hold our mesh data
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, host_VertexBuffer.size() * sizeof(float),
                 host_VertexBuffer.data(), GL_STATIC_DRAW);

    // create a vertex array object that will map the attributes in
    // our vertex buffer to different location attributes for our
    // shaders
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glEnableVertexAttribArray(0); // This is the position attribute
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr); // 6 floats per vertex: position + normal

    glEnableVertexAttribArray(1); // This is the normal attribute
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        reinterpret_cast<void*>(3 * sizeof(GLfloat))
    );

    // The index buffer binding belongs to the VAO, so bind it while the VAO is
    // bound or glDrawElements will not find it.
    glGenBuffers(1, &m_indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int),
                 indexBuffer.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Three programs share the same vertex shader. The vertex stage hands the
    // world-space normal, light direction, and position to the fragment stage,
    // so only the fragment programs differ between shading models.
    const char* prepVertexShader = "vertexShader_PrepForPerFragment.glsl";
    const char* fragmentShaders[NumShaders] = {
        "fragmentShader_Lambertian.glsl",
        "fragmentShader_BlinnPhong.glsl",
        "fragmentShader_Normal.glsl",
    };

    sivelab::GLSLObject shaders[NumShaders];
    ShaderUniforms uniforms[NumShaders];

    for (int i = 0; i < NumShaders; i++) {
        shaders[i].addShader( resolveShaderPath(prepVertexShader).c_str(), sivelab::GLSLObject::VERTEX_SHADER );
        shaders[i].addShader( resolveShaderPath(fragmentShaders[i]).c_str(), sivelab::GLSLObject::FRAGMENT_SHADER );
        shaders[i].createProgram();

        uniforms[i].projMatrixID = shaders[i].createUniform("projMatrix");
        uniforms[i].viewMatrixID = shaders[i].createUniform("viewMatrix");
        uniforms[i].modelMatrixID = shaders[i].createUniform("modelMatrix");
        uniforms[i].normalMatrixID = shaders[i].createUniform("normalMatrix");
        uniforms[i].lightPosWorldID = shaders[i].createUniform("lightPosWorld");
        uniforms[i].diffuseComponentID = shaders[i].createUniform("diffuseComponent");
        uniforms[i].specularComponentID = shaders[i].createUniform("specularComponent");
        uniforms[i].phongExponentID = shaders[i].createUniform("phongExponent");
        uniforms[i].eyePosWorldID = shaders[i].createUniform("eyePosWorld");
    }

    // Adds a point light in front of the triangle, on the same side as the camera.
    const glm::vec4 lightPosWorld(0.0f, 1.0f, 3.0f, 1.0f);
    const glm::vec3 diffuseComponent(0.8f, 0.3f, 0.8f); // Set color of the triangle here for non-normal.
    const glm::vec3 specularComponent(1.0f, 1.0f, 1.0f);
    const float phongExponent = 32.0f;

    std::cout << "Shading keys: 1 = Lambertian, 2 = Blinn-Phong, 3 = Normal\n";

    // Model transform state, changed every frame (spin) and by the keyboard.
    glm::vec3 modelTranslation(0.0f, 0.0f, 0.0f);
    float modelScale = 1.0f;
    float modelAngle = 0.0f;
    bool spinning = false;
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
    int shownShader = -1;
    
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

        // Spin about the triangle's Y axis when enabled (R toggles it). This
        // makes the changing normal visible in the normal shader.
        if (spinning) {
            modelAngle += spinRatePerSecond * static_cast<float>(timeDiff);
        }

        // Scale, then rotate about the triangle's own Y axis, then translate.
        glm::mat4 M_model = glm::translate(glm::mat4(1.0f), modelTranslation);
        M_model = glm::rotate(M_model, modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        M_model = glm::scale(M_model, glm::vec3(modelScale));

        // Below is a note for me to remember about Normals.

        // Normals are directions. The inverse-transpose of the model matrix
        // keeps them perpendicular to the surface after non-uniform scale.
        // lightDir in the vertex shader is in world space, so this matrix
        // must take normals to world space too (model, not model-view).
        glm::mat4 M_normal = glm::transpose(glm::inverse(M_model));

        // Select the active shading program and upload this frame's uniforms.
        sivelab::GLSLObject& shader = shaders[activeShader];
        const ShaderUniforms& u = uniforms[activeShader];

        if (shownShader != activeShader) {
            const std::string title = std::string("GLFW Example - ") + shaderNames[activeShader];
            glfwSetWindowTitle(window, title.c_str());
            shownShader = activeShader;
        }

        shader.activate();

        // copy from the host to the device the projection, view, and model matrices
        glUniformMatrix4fv(u.projMatrixID, 1, GL_FALSE, glm::value_ptr( M_proj ));
        glUniformMatrix4fv(u.viewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(u.modelMatrixID, 1, GL_FALSE, glm::value_ptr( M_model ));
        glUniformMatrix4fv(u.normalMatrixID, 1, GL_FALSE, glm::value_ptr( M_normal ));

        const glm::vec4 eyePosWorld(m_pos, 1.0f);
        glUniform4fv(u.lightPosWorldID, 1, glm::value_ptr(lightPosWorld));
        glUniform4fv(u.eyePosWorldID, 1, glm::value_ptr(eyePosWorld));
        glUniform3fv(u.diffuseComponentID, 1, glm::value_ptr(diffuseComponent));
        glUniform3fv(u.specularComponentID, 1, glm::value_ptr(specularComponent));
        glUniform1f(u.phongExponentID, phongExponent);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, obj.getNumberOfIndices(), GL_UNSIGNED_INT, nullptr);
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

        //   1 / 2 / 3 — Lambertian / Blinn-Phong / Normal shading
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            activeShader = LAMBERTIAN;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            activeShader = BLINN_PHONG;
        }
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            activeShader = NORMAL_SHADER;
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
