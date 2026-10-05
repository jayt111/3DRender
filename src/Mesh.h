#pragma once
// Class to store meshes
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Texture.h"
#include "shader.h"

// Helper Structs
struct Vertex {
  glm::vec3 pos;      // Position
  glm::vec3 nor;      // Normal
  glm::vec2 texCoord; // Texture Coordinate
  glm::vec3 tangent;  // Tangent
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
public:
  // Data
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;
  GLuint vao = 0;

  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
       std::vector<Texture> textures, bool isProcedural, float transparency);
  ~Mesh();

  void draw(Shader &shader) const;
  void drawShadow() const;

private:
  bool isProcedural = false;
  float transparency = 0.f;
  void setup();
  GLuint vbo = 0, ebo = 0; // Vertex and Element Buffer Objects
};
