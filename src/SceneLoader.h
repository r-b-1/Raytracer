#pragma once

#include <stdexcept>
#include <unordered_map>

#include "ISceneLoader.h"
#include "blinn_phong_shader.h"
#include "lambertian_shader.h"
#include "mirror_shader.h"
#include "model_obj.h"
#include "orthographic_camera.h"
#include "perspective_camera.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"

class SceneLoader : public ISceneLoader {
private:
  Scene &m_targetScene; // Reference to the external scene
  std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;

  static Vec3 toVec3(const ISceneLoader::vec &v) {
    return Vec3(v.x, v.y, v.z);
  }

public:
  // The caller provides the scene to be filled
  SceneLoader(Scene &sceneToPopulate)
      : m_targetScene(sceneToPopulate), numShaders(0), numTextures(0) {}

  void reserveCameras(size_t count) override {
    // makes sure the vector has enough space
    m_targetScene.cameras.reserve(count);
  }
  void reserveLights(size_t count) override {
    m_targetScene.lights.reserve(count);
  }
  void reserveShapes(size_t count) override {
    m_targetScene.objects.reserve(count);
  }
  void reserveShaders(size_t count) override {
    numShaders = count;
    m_shaders.reserve(count);
  }
  void reserveTextures(size_t count) override { numTextures = count; }

  void setSceneParams(const SceneParams &sparams) override {
    if (!sparams.envMapFile.empty())
      throw std::runtime_error("Environment maps are not supported by SceneLoader");
    m_targetScene.background_color = toVec3(sparams.bgColor);
  }

  void addCamera(const std::string &name, const std::string &type,
                 const ISceneLoader::vec &pos, const ISceneLoader::vec &viewDir,
                 float focalLength, float imagePlaneWidth) override {
    std::shared_ptr<Camera> cam;
    if (type == "perspective")
      cam = std::make_shared<PerspectiveCamera>();
    else if (type == "orthographic")
      cam = std::make_shared<OrthographicCamera>();
    else
      throw std::runtime_error("Unsupported camera type: " + type);

    cam->lookfrom = toVec3(pos);
    // The API supplies a direction, while Camera stores a target point.
    cam->lookat = cam->lookfrom + toVec3(viewDir);
    cam->focal_length = focalLength;
    cam->image_plane_width = imagePlaneWidth;
    m_targetScene.cameras.push_back(cam);
  }
  void addPointLight(const ISceneLoader::vec &pos,
                     const ISceneLoader::vec &intensity) override {
    m_targetScene.lights.emplace_back(toVec3(pos), toVec3(intensity));
  }
  void addAreaLight(const ISceneLoader::vec &pos,
                    const ISceneLoader::vec &color,
                    const ISceneLoader::vec &normal, float width,
                    float length) override {
    throw std::runtime_error("Area lights are not supported by SceneLoader");
  }
  void addShader(const ISceneLoader::ShaderDesc &shaderDesc) override {
    if (shaderDesc.diffuse.hasTexture || shaderDesc.specular.hasTexture ||
        shaderDesc.normal.hasTexture || shaderDesc.emission.hasTexture)
      throw std::runtime_error("Textured shaders are not supported by SceneLoader");

    // The parser loads lights before shaders. Materials copy these lights.
    std::shared_ptr<Shader> shader;
    if (shaderDesc.type == "Lambertian" || shaderDesc.type == "Diffuse") {
      shader = std::make_shared<LambertianShader>(
          toVec3(shaderDesc.diffuse.data), m_targetScene.lights);
    } else if (shaderDesc.type == "BlinnPhong" || shaderDesc.type == "Phong") {
      shader = std::make_shared<BlinnPhongShader>(
          toVec3(shaderDesc.diffuse.data), m_targetScene.lights,
          toVec3(shaderDesc.specular.data), shaderDesc.phongExp);
    } else if (shaderDesc.type == "Mirror") {
      if (shaderDesc.roughness != 0.0f)
        throw std::runtime_error("Rough mirrors are not supported by SceneLoader");
      shader = std::make_shared<MirrorShader>(Color(1, 1, 1), m_targetScene.lights);
    } else if (shaderDesc.type == "Normal") {
      shader = std::make_shared<NormalShader>();
    } else {
      throw std::runtime_error("Unsupported shader type: " + shaderDesc.type);
    }
    m_shaders[shaderDesc.name] = shader;
  }
  void addShape(const ISceneLoader::ShapeDesc &shapeDesc) override {
    if (shapeDesc.hasXForm)
      throw std::runtime_error("Shape transforms are not supported by SceneLoader");

    std::shared_ptr<Shader> shader = std::make_shared<NormalShader>();
    if (!shapeDesc.shaderNameReference.empty()) {
      const auto found = m_shaders.find(shapeDesc.shaderNameReference);
      if (found == m_shaders.end())
        throw std::runtime_error("Unknown shader reference: " + shapeDesc.shaderNameReference);
      shader = found->second;
    }

    if (shapeDesc.type == "sphere") {
      m_targetScene.objects.push_back(std::make_shared<Sphere>(
          toVec3(shapeDesc.center), shapeDesc.radius, shader));
    } else if (shapeDesc.type == "triangle") {
      m_targetScene.objects.push_back(std::make_shared<Triangle>(
          toVec3(shapeDesc.v0), toVec3(shapeDesc.v1), toVec3(shapeDesc.v2), shader));
    } else if (shapeDesc.type == "mesh") {
      ModelOBJ mesh;
      if (!mesh.import(shapeDesc.meshFilePath.c_str()) || mesh.getNumberOfTriangles() == 0)
        throw std::runtime_error("Failed to load OBJ mesh: " + shapeDesc.meshFilePath);
      const int *indices = mesh.getIndexBuffer();
      const auto vertex = [&mesh](int index) {
        const auto &p = mesh.getVertex(index).position;
        return Point3(p[0], p[1], p[2]);
      };
      for (int i = 0; i < mesh.getNumberOfIndices(); i += 3) {
        m_targetScene.objects.push_back(std::make_shared<Triangle>(
            vertex(indices[i]), vertex(indices[i + 1]), vertex(indices[i + 2]), shader));
      }
    } else {
      throw std::runtime_error("Unsupported shape type: " + shapeDesc.type);
    }
  }
  void addTexture(const std::string &type, const std::string &name,
                  const std::string &sourceFile) override {
    throw std::runtime_error("Textures are not supported by SceneLoader");
  }

public:
  int numShaders;
  int numTextures;
};
