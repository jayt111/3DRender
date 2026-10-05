#pragma once

#include "GL/gl3w.h"
#include "Light.h"
#include "SceneNode.h"
#include "Skybox.h"
#include "shader.h"
#include <shadow.h>
#include <vector>

class Scene {
public:
  void addRoot(SceneNode *node);

  void drawAll(Shader &shader, Shader &particleShader, const glm::mat4 &view,
               const glm::mat4 projection, const glm::vec3 camPos);

  void drawAllShadow(Shader &shadowShader,
                     const glm::mat4 &projectedLightSpaceMatrix);

  void collectTransparentNodes(
      SceneNode *node, const glm::vec3 &camPos,
      std::vector<std::pair<float, SceneNode *>> &transparentNodes);

private:
  std::vector<SceneNode *> roots;
  void uploadLights(Shader &shader, std::vector<Light> &lights);
};
