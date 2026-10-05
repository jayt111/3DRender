#pragma once

#include "Mesh.h"
#include <memory>

class Procedual {
public:
  Procedual(int width, int depth, float scale);

  std::shared_ptr<Mesh> generateMesh();

private:
  int width, depth;
  float scale;

  std::vector<Vertex> generateVertices();
  std::vector<unsigned int> generateIndices();
  std::vector<Texture> generateTextures();
  void textureHelper(std::vector<Texture> &textures, const std::string &path,
                     const std::string &type);
  void computeNormals(std::vector<Vertex> &vertices,
                      const std::vector<unsigned int> &indices);
  float getHeight(int x, int z) const;
};
