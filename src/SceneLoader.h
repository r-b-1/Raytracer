#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ISceneLoader.h"
#include "scene.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"

#include "lambertian_shader.h"
#include "blinn_phong_shader.h"
#include "mirror_shader.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"

class SceneLoader : public ISceneLoader {
private:
  Scene &m_targetScene; // Reference to the external scene

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
  void reserveShaders(size_t count) override { numShaders = count; }
  void reserveTextures(size_t count) override { numTextures = count; }

  void setSceneParams(const SceneParams &sparams) override {
    std::cout << "Setting scene params." << std::endl;
  }

  void addCamera(const std::string &name, const std::string &type,
                 const ISceneLoader::vec &pos, const ISceneLoader::vec &viewDir,
                 float focalLength, float imagePlaneWidth) override {
    std::cout << "Creating camera: " << name << ", type:" << type << std::endl;
    std::shared_ptr<Camera> cam;
    if (type == "perspective")
      cam = std::make_shared<PerspectiveCamera>();
    else if (type == "orthographic")
      cam = std::make_shared<OrthographicCamera>();
    else
      throw std::runtime_error("Unknown camera type: " + type);
    cam->lookfrom = Point3(pos.x, pos.y, pos.z);
    cam->lookat = Point3(pos.x + viewDir.x, pos.y + viewDir.y, pos.z + viewDir.z);
    cam->focal_length = focalLength;
    // Camera sizes the image plane by height; this matches the JSON width for square images.
    cam->image_plane_height = imagePlaneWidth;
    m_targetScene.cameras.push_back(cam);
  }
  void addPointLight(const ISceneLoader::vec &pos,
                     const ISceneLoader::vec &intensity) override {
    std::cout << "Creating point light." << std::endl;
    auto light = std::make_shared<PointLight>(
        Point3(pos.x, pos.y, pos.z),
        Color(intensity.x, intensity.y, intensity.z));
    m_targetScene.lights.push_back(light);
  }
  void addAreaLight(const ISceneLoader::vec &pos,
                    const ISceneLoader::vec &color,
                    const ISceneLoader::vec &normal, float width,
                    float length) override {
    std::cout << "Creating area light." << std::endl;
  }
  void addShader(const ISceneLoader::ShaderDesc &shaderDesc) override {
    std::cout << "Creating shader: type=" << shaderDesc.type << std::endl;
    const auto &d = shaderDesc.diffuse.data;
    const auto &s = shaderDesc.specular.data;
    const Color diffuse(d.x, d.y, d.z);
    // Lights are parsed before shaders, so the scene's lights are already loaded.
    const auto &lights = m_targetScene.lights;

    std::shared_ptr<Shader> shader;
    if (shaderDesc.type == "Lambertian" || shaderDesc.type == "Diffuse") {
      shader = std::make_shared<LambertianShader>(diffuse, lights);
    } else if (shaderDesc.type == "BlinnPhong" || shaderDesc.type == "Phong") {
      shader = std::make_shared<BlinnPhongShader>(
          diffuse, lights, Color(s.x, s.y, s.z), shaderDesc.phongExp);
    } else if (shaderDesc.type == "Mirror") {
      shader = std::make_shared<MirrorShader>(Color(1, 1, 1), lights);
    } else {
      throw std::runtime_error("Unknown shader type: " + shaderDesc.type);
    }
    m_targetScene.shaders.push_back(shader);
    m_shadersByName[shaderDesc.name] = shader;
  }
  void addShape(const ISceneLoader::ShapeDesc &shapeDesc) override {
    std::shared_ptr<Shader> shader = std::make_shared<NormalShader>();
    if (!shapeDesc.shaderNameReference.empty()) {
      auto it = m_shadersByName.find(shapeDesc.shaderNameReference);
      if (it == m_shadersByName.end())
        throw std::runtime_error("Unknown shader reference: " +
                                 shapeDesc.shaderNameReference);
      shader = it->second;
    }

    if (shapeDesc.type == "sphere") {
      m_targetScene.objects.push_back(std::make_shared<Sphere>(
        Point3(shapeDesc.center.x, shapeDesc.center.y, shapeDesc.center.z),
        shapeDesc.radius, shader));
    } else if (shapeDesc.type == "triangle") {
      m_targetScene.objects.push_back(std::make_shared<Triangle>(
        Point3(shapeDesc.v0.x, shapeDesc.v0.y, shapeDesc.v0.z),
        Point3(shapeDesc.v1.x, shapeDesc.v1.y, shapeDesc.v1.z),
        Point3(shapeDesc.v2.x, shapeDesc.v2.y, shapeDesc.v2.z), shader));
    } else {
      std::cout << "Creating shape: type=" << shapeDesc.type << std::endl;
    }
  }
  void addTexture(const std::string &type, const std::string &name,
                  const std::string &sourceFile) override {
    std::cout << "Creating texture: type=" << type << ", name=" << name
              << ", sourceFile=" << sourceFile << std::endl;
  }

public:
  int numShaders;
  int numTextures;

private:
  std::unordered_map<std::string, std::shared_ptr<Shader>> m_shadersByName;
};
