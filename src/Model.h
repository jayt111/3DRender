#pragma once
#include <memory>
#include <shader.h>
#include <string>
#include <vector>

#include "Mesh.h"
#include "tiny_obj_loader.h"

class Model {
public:
  Model(const std::string &path, float transparency = 0.f);
  Model(std::shared_ptr<Mesh> mesh);
  ~Model();

  void draw(Shader &shader) const;
  void drawShadow() const;

private:
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::string directory;

  void loadFromObj(const std::string &filepath, float transparency);
  std::vector<Texture> loadMaterialTextures(const tinyobj::material_t &mat);
  void loadTextureHelper(std::vector<Texture> &textures,
                         const std::string &texPath,
                         const std::string &type) const;
  void computeTangents(std::vector<Vertex> &vertices,
                       const std::vector<GLuint> &indices);
};
