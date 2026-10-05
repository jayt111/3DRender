/*
* Mesh.cpp
* 
* Holds vertex data, textures and setups the OpenGL buffers for rendering
* Supports drawing with a standard shader and rendering shadows
* 
* Uses glDrawElements to render the mesh using indexed drawing
*
*/

#include "Mesh.h"

#include <iostream>
#include <GLFW/glfw3.h>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, bool isProcedural, float transparency) 
	: isProcedural(isProcedural), transparency(transparency)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setup();
}

// Cleanup
Mesh::~Mesh()
{
	// Delete buffers
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	
	// Delete Textures
	for (GLuint i = 0; i < textures.size(); i++) {
		glDeleteTextures(1, &textures[i].id);
	}
}

void Mesh::draw(Shader& shader) const
{
	bool hasNormal = false;
	bool hasMetal = false;
	bool hasRough = false;
	bool hasAo = false;
	for (GLuint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string name = textures[i].type;
		if (name == "normalMap")
			hasNormal = true;
		if (name == "metallicMap")
			hasMetal = true;
		if (name == "roughnessMap")
			hasRough = true;
		if (name == "aoMap")
			hasAo = true;
		// set sampler
		shader.setInt(name, i);
		// Bind texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	shader.setBool("hasNormal", hasNormal);
	shader.setBool("hasMetal", hasMetal);
	shader.setBool("hasRough", hasRough);
	shader.setBool("hasAo", hasAo);

	shader.setBool("isProcedural", isProcedural);

	shader.setFloat("transparency", transparency);
	
	// draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// set back to default
	glActiveTexture(GL_TEXTURE0);

}

void Mesh::drawShadow() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setup()
{
	// Send data to GPU
	// Generate buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	
	// Bind Element Buffer 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// location 0 position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(0);
	
	// location 1 normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nor));
	glEnableVertexAttribArray(1);
	
	// location 2 texture coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	// location 3 tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);
		
	glBindVertexArray(0);
}
