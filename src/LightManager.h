#pragma once

#include "Light.h"
#include "Scene.h"
#include <memory>
#include <shader.h>

class LightManager {
public:
  void addLight(const Light &light);
  void updateLight(int index, const glm::vec3 &pos, const glm::vec3 &dir);

  void updateLightColour(int index, const glm::vec3 &col);

  void uploadLightsToShader(Shader &shader);
  void bindShadowMaps(Shader &shader, GLuint startingTextureUnit = 22) const;
  void generateShadowMaps(Scene &scene, Shader &shadowShader, int width,
                          int height) const;

  const std::vector<Light> &getLights() const;
  Light &getLight(int index);

private:
  std::vector<Light> lights;
};
