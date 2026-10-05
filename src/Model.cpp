#include "Model.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


Model::Model(const std::string& path, float transparency)
{
	loadFromObj(path, transparency);
}

Model::Model(std::shared_ptr<Mesh> mesh) {
	meshes.push_back(mesh);
}

Model::~Model()
{
}

void Model::draw(Shader& shader) const
{
	for (const auto& mesh : meshes) {
		mesh->draw(shader);
	}
}

void Model::drawShadow() const
{
	for (const auto& mesh : meshes) {
		mesh->drawShadow();
	}
}

void Model::loadFromObj(const std::string& filepath, float transparency)
{

	std::cout << "[Model] Loading Model " << filepath << " ..." << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	// get base directory
	directory = filepath.substr(0, filepath.find_last_of('/'));

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(), directory.c_str(), true)) {
		if (!warn.empty())
			std::cout << "[Model] TinyObj Warning: " << warn << std::endl;
		if (!err.empty())
			std::cout << "[Model] TinyObj Error: " << err << std::endl;
		return;
	}


	for (const auto& shape : shapes) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
			auto& index = shape.mesh.indices[i];
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (!attrib.normals.empty() && index.normal_index >= 0) {
				vertex.nor = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}
			
			if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}
		
			vertices.push_back(vertex);
			indices.push_back((GLuint)i);
		}

		// Calculate Tangents
		computeTangents(vertices, indices);

		// Materials
		int matID = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];
		if (matID >= 0 && matID < materials.size()) {
			std::cout << "[Model] Shape: " << shape.name << " uses Material: " << materials[matID].name << std::endl;
			textures = loadMaterialTextures(materials[matID]);
		}
		
		// Create mesh object
		auto mesh = std::make_shared<Mesh>(vertices, indices, textures, false, transparency);
		// Add to list of meshes
		meshes.push_back(mesh);
	}
}

std::vector<Texture> Model::loadMaterialTextures(const tinyobj::material_t& mat)
{
	std::vector<Texture> textures;

	loadTextureHelper(textures, mat.diffuse_texname, "albedoMap");
	loadTextureHelper(textures, mat.normal_texname, "normalMap");
	loadTextureHelper(textures, mat.metallic_texname, "metallicMap");
	loadTextureHelper(textures, mat.roughness_texname, "roughnessMap");
	loadTextureHelper(textures, mat.ambient_texname, "aoMap");

	return textures;
}

void Model::loadTextureHelper(std::vector<Texture>& textures, const std::string& texPath, const std::string& type) const
{
	if (!texPath.empty()) {
		std::string fullPath = directory + "/" + texPath;
		std::cout << "[Model] Loading " << type << ": " << fullPath << std::endl;

		Texture t;
		t.id = LoadTexture(fullPath.c_str());
		t.type = type;
		t.path = texPath;
		textures.push_back(t);
	}
}

void Model::computeTangents(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
	// Initialise to 0
	for (auto& v : vertices) {
		v.tangent = glm::vec3(0.f);
	}
	
	// Calculate Tangents
	for (size_t i = 0; i < indices.size(); i += 3) {
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		glm::vec3 pos1 = v0.pos;
		glm::vec3 pos2 = v1.pos;
		glm::vec3 pos3 = v2.pos;

		glm::vec2 uv1 = v0.texCoord;
		glm::vec2 uv2 = v1.texCoord;
		glm::vec2 uv3 = v2.texCoord;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
		f = (f == 0.f) ? 1.0f : 1.0f / f;

		glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);

		glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

		v0.tangent = tangent;
		v1.tangent = tangent;
		v2.tangent = tangent;
	}

	// Normalise tangents
	for (auto& v : vertices) {
		v.tangent = glm::normalize(v.tangent);
	}
}
