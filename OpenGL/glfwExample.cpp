#include <cmath>
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

// A mesh on the GPU: interleaved position+normal vertices and an index buffer,
// wrapped in a VAO so drawing it is one glDrawElements call.
struct GLMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};

GLMesh uploadMesh(const std::vector<float>& vertices,
                  const std::vector<unsigned int>& indices) {
    GLMesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);

    // The element buffer binding belongs to the VAO, so bind it while the VAO
    // is bound or glDrawElements will not find it.
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    mesh.indexCount = static_cast<GLsizei>(indices.size());
    return mesh;
}

// Sphere triangles are emitted as a list (position = normal on the unit
// sphere); the radius comes from the model matrix at draw time.
void addSphereTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
                       std::vector<float>& vertices) {
    const glm::vec3 corners[3] = { a, b, c };
    for (const glm::vec3& v : corners)
        vertices.insert(vertices.end(), { v.x, v.y, v.z, v.x, v.y, v.z });
}

// Week 11 lecture: split a triangle into four and push the edge midpoints out
// onto the sphere, then recurse. A latitude/longitude sphere would pinch at the
// poles; starting from an icosahedron distributes the vertices evenly.
void subdivideSphere(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, int depth,
                     std::vector<float>& vertices) {
    if (depth == 0) {
        addSphereTriangle(v0, v1, v2, vertices); // record the triangle
        return;
    }

    const glm::vec3 v0v1 = glm::normalize((v0 + v1) * 0.5f);
    const glm::vec3 v1v2 = glm::normalize((v1 + v2) * 0.5f);
    const glm::vec3 v2v0 = glm::normalize((v2 + v0) * 0.5f);

    subdivideSphere(v0, v0v1, v2v0, depth - 1, vertices);
    subdivideSphere(v0v1, v1, v1v2, depth - 1, vertices);
    subdivideSphere(v2v0, v1v2, v2, depth - 1, vertices);
    subdivideSphere(v0v1, v1v2, v2v0, depth - 1, vertices);
}

// Builds a unit sphere from an icosahedron (12 vertices, 20 triangles) by
// recursive subdivision. depth 3 is 20 * 4^3 = 1280 triangles.
void buildSphere(int depth, std::vector<float>& vertices) {
    const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f;
    const glm::vec3 corners[12] = {
        { -1, phi, 0 }, { 1, phi, 0 }, { -1, -phi, 0 }, { 1, -phi, 0 },
        { 0, -1, phi }, { 0, 1, phi }, { 0, -1, -phi }, { 0, 1, -phi },
        { phi, 0, -1 }, { phi, 0, 1 }, { -phi, 0, -1 }, { -phi, 0, 1 },
    };
    const int faces[20][3] = {
        { 0, 11, 5 }, { 0, 5, 1 }, { 0, 1, 7 }, { 0, 7, 10 }, { 0, 10, 11 },
        { 1, 5, 9 }, { 5, 11, 4 }, { 11, 10, 2 }, { 10, 7, 6 }, { 7, 1, 8 },
        { 3, 9, 4 }, { 3, 4, 2 }, { 3, 2, 6 }, { 3, 6, 8 }, { 3, 8, 9 },
        { 4, 9, 5 }, { 2, 4, 11 }, { 6, 2, 10 }, { 8, 6, 7 }, { 9, 8, 1 },
    };

    for (const auto& face : faces) {
        subdivideSphere(glm::normalize(corners[face[0]]),
                        glm::normalize(corners[face[1]]),
                        glm::normalize(corners[face[2]]),
                        depth, vertices);
    }
}

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

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;

    // Load the bunny mesh: center it and scale its longest side to 6 units.
    ModelOBJ obj;
    if (!obj.import("assets/bunny.obj")) {
        std::cerr << "Could not load assets/bunny.obj (run from the project root)." << std::endl;
        glfwTerminate();
        return -1;
    }
    obj.normalize(6.0f, true);

    // Pack position (xyz) followed by normal (xyz) per vertex, the layout the
    // shaders expect.
    std::vector< float > bunnyVertices;
    bunnyVertices.reserve(obj.getNumberOfVertices() * 6);
    for (int i = 0; i < obj.getNumberOfVertices(); i++) {
        const ModelOBJ::Vertex& v = obj.getVertex(i);
        bunnyVertices.insert(bunnyVertices.end(),
                             { v.position[0], v.position[1], v.position[2],
                               v.normal[0], v.normal[1], v.normal[2] });
    }
    std::vector< unsigned int > bunnyIndices(
        obj.getIndexBuffer(), obj.getIndexBuffer() + obj.getNumberOfIndices());

    GLMesh bunny = uploadMesh(bunnyVertices, bunnyIndices);

    // Spheres are triangle meshes too: subdivide an icosahedron and project the
    // new vertices onto the sphere (Week 11 lecture). depth 3 is 1280 triangles.
    std::vector< float > sphereVertices;
    buildSphere(3, sphereVertices);

    std::vector< unsigned int > sphereIndices(sphereVertices.size() / 6);
    for (std::size_t i = 0; i < sphereIndices.size(); ++i)
        sphereIndices[i] = static_cast<unsigned int>(i);

    GLMesh sphereMesh = uploadMesh(sphereVertices, sphereIndices);

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

    // Spheres in front of the bunny. They all share the one triangle mesh
    // above; the model matrix supplies position and radius.
    struct SphereInstance {
        glm::vec3 center;
        float radius;
        glm::vec3 color;
    };
    const SphereInstance spheres[] = {
        { glm::vec3(-2.9f, 3.0f, 2.2f), 0.85f, glm::vec3(0.85f, 0.25f, 0.25f) },
        { glm::vec3(0.0f, -2.5f, 3.2f), 0.65f, glm::vec3(0.25f, 0.75f, 0.35f) },
        { glm::vec3(2.9f, 0.0f, 2.2f), 0.95f, glm::vec3(0.25f, 0.45f, 0.9f) },
    };

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

        glBindVertexArray(bunny.vao);
        glDrawElements(GL_TRIANGLES, bunny.indexCount, GL_UNSIGNED_INT, nullptr);

        // Each sphere reuses the same triangle mesh, moved and scaled.
        glBindVertexArray(sphereMesh.vao);
        for (const SphereInstance& sphere : spheres) {
            const glm::mat4 M_sphere =
                glm::translate(glm::mat4(1.0f), sphere.center)
                * glm::scale(glm::mat4(1.0f), glm::vec3(sphere.radius));
            const glm::mat4 N_sphere = glm::transpose(glm::inverse(M_sphere));

            glUniformMatrix4fv(u.modelMatrixID, 1, GL_FALSE, glm::value_ptr(M_sphere));
            glUniformMatrix4fv(u.normalMatrixID, 1, GL_FALSE, glm::value_ptr(N_sphere));
            glUniform3fv(u.diffuseComponentID, 1, glm::value_ptr(sphere.color));

            glDrawElements(GL_TRIANGLES, sphereMesh.indexCount, GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);

        /* Poll for and process events */
        glfwPollEvents();

        // float moveRatePerFrame = 0.05;
        float moveSpeedPerSecond = 5.0f; // Changed it to be per second instead of per frame because on my PC I was moving too fast.
        float moveStep = moveSpeedPerSecond * static_cast<float>(timeDiff);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_pos = m_pos + -m_W * moveStep;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_pos = m_pos - m_U * moveStep;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_pos = m_pos + m_W * moveStep;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_pos = m_pos + m_U * moveStep;
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
            modelTranslation.x -= moveStep;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            modelTranslation.x += moveStep;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            modelTranslation.y += moveStep;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            modelTranslation.y -= moveStep;
        }

        float scaleStep = 0.3f * static_cast<float>(timeDiff);

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            modelScale = glm::max(0.1f, modelScale - scaleStep);
        }
        else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            modelScale += scaleStep;
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
