#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "shader.h"
#include <vector>

class Skybox {
public:
  Skybox();
  ~Skybox();

  void draw(Shader &shader, const glm::mat4 &view,
            const glm::mat4 &projection) const;

private:
  void setup();

  GLuint vao = 0, vbo = 0;
  GLuint textureID = 0;

  std::vector<std::string> faces{
      "Skybox/px.png", "Skybox/nx.png", "Skybox/py.png",
      "Skybox/ny.png", "Skybox/pz.png", "Skybox/nz.png",
  };
};
