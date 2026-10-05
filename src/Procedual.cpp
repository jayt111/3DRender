#include "Procedual.h"

#include <glm/gtc/noise.hpp>

Procedual::Procedual(int width, int depth, float scale)
	: width(width), depth(depth), scale(scale)
{
}

std::shared_ptr<Mesh> Procedual::generateMesh()
{
	std::vector<Vertex> vertices = generateVertices();
	std::vector<unsigned int> indices = generateIndices();
	computeNormals(vertices, indices);
	std::vector<Texture> textures = generateTextures();

	return std::make_shared<Mesh>(vertices, indices, textures, true, 0);
}

std::vector<Vertex> Procedual::generateVertices()
{
	std::vector<Vertex> vertices;

	for (int z = 0; z <= depth; z++) {
		for (int x = 0; x <= width; x++) {
			float fx = (float)x * scale;
			float fz = (float)z * scale;
			
			float fy = getHeight(fx, fz) * scale;

			Vertex v;
			v.pos = glm::vec3(fx, fy, fz);
			//v.texCoord = glm::vec2(fx / width, fz / depth);
			// Tile texture
			float tileFactor = 30.f;
			v.texCoord = glm::vec2(fx, fz) * tileFactor / glm::vec2(width, depth);

			v.nor = glm::vec3(0.f); // calculate later
			
			vertices.push_back(v);
		}
	}

	return vertices;
}

std::vector<unsigned int> Procedual::generateIndices()
{
	std::vector<unsigned int> indices;
	
	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			int i = z * (width + 1) + x;
			indices.push_back(i);
			indices.push_back(i + width + 1);
			indices.push_back(i + 1);

			indices.push_back(i + 1);
			indices.push_back(i + width + 1);
			indices.push_back(i + width + 2);
		}
	}
	return indices;
}

std::vector<Texture> Procedual::generateTextures()
{
	std::vector<Texture> textures;
	textureHelper(textures, "Objects/MountainTextures/Grass/grass.jpg", "grassAlbedo");
	textureHelper(textures, "Objects/MountainTextures/Rock/rock.jpg", "rockAlbedo");
	textureHelper(textures, "Objects/MountainTextures/Snow/snow.jpg", "snowAlbedo");
	
	textureHelper(textures, "Objects/MountainTextures/Grass/grassNormal.png", "grassNormal");
	textureHelper(textures, "Objects/MountainTextures/Rock/rockNormal.png", "rockNormal");
	textureHelper(textures, "Objects/MountainTextures/Snow/snowNormal.png", "snowNormal");

	textureHelper(textures, "Objects/MountainTextures/Grass/grassRoughness.jpg", "grassRough");
	textureHelper(textures, "Objects/MountainTextures/Rock/rockRoughness.jpg", "rockRough");
	textureHelper(textures, "Objects/MountainTextures/Snow/snowRoughness.jpg", "snowRough");

	textureHelper(textures, "Objects/MountainTextures/Grass/grassAo.jpg", "grassAo");
	textureHelper(textures, "Objects/MountainTextures/Rock/rockAo.jpg", "rockAo");
	textureHelper(textures, "Objects/MountainTextures/Snow/snowAo.jpg", "snowAo");
	
	return textures;
}

void Procedual::textureHelper(std::vector<Texture>& textures, const std::string& path, const std::string& type)
{
	std::cout << "[Procedural] Loading Texture: " << path << std::endl;
	Texture t;
	t.id = LoadTexture(path.c_str());
	t.type = type;
	t.path = path;
	
	textures.push_back(t);
}

void Procedual::computeNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	for (size_t i = 0; i < indices.size(); i += 3) {
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		v0.nor += normal;
		v1.nor += normal;
		v2.nor += normal;
	}

	for (auto& v : vertices) {
		v.nor = glm::normalize(v.nor);
	}
}

float Procedual::getHeight(int x, int z) const
{
	float frequency = 0.02f; // zoom of noise
	float amplitude = 40.f;	// max height of terrain
	glm::vec2 pos = glm::vec2(x, z) * frequency;
	
	// Create a flat plateau in the middle which is blended into the noise
	float centerX = width / 2.f;
	float centerZ = width / 2.f;
	float plateauSize = 50.f;
	float blendRadius = 60.f;

	float dx = 0.f;
	if (x < centerX - plateauSize)
		dx = (centerX - plateauSize) - x;
	else if (x > centerX + plateauSize)
		dx = x - (centerX + plateauSize);


	float dz = 0.f;
	if (z < centerZ - plateauSize)
		dz = (centerZ - plateauSize) - z;
	else if (z > centerZ + plateauSize)
		dz = z - (centerX + plateauSize);
	
	float dist = sqrt(dx * dx + dz * dz);


	float blend = glm::clamp(dist / blendRadius, 0.f, 1.f);

	blend = blend * blend * (3 - 2 * blend);

	float noiseHeight = glm::perlin(pos) * amplitude;

	float result = blend * noiseHeight;
	
	return result;
}
