/*
* Texture.cpp
* 
* A slightly modidied version of the supplied Texture.h but split into a header a source file to stop include errors
*/

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <iostream>
#include <vector>

GLuint LoadTexture(const char* path) {

	std::cout << "[Texture] Loading: " << path << " ..." << std::endl;

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "[Texture] Failed to load texture " << path << std::endl;
		stbi_image_free(data);
		return -1;
	}

	GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
	GLenum internalFormat = format;
	if (channels == 1) {
		format = GL_RED;
		internalFormat = GL_R8;
	}
	std::cout << "channels: " << channels << std::endl;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_image_free(data);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return tex;
}

// Modified from https://learnopengl.com/Advanced-OpenGL/Cubemaps
GLuint LoadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, channels;

	for (GLuint i = 0; i < faces.size(); i++) {
		std::cout << "[Texture] Loading cubemap texture: " << faces[i] << std::endl;

		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
		if (!data) {
			std::cerr << "[Texture] Failed to load cubemap texture: " << faces[i] << std::endl;
			stbi_image_free(data);
			return -1;
		}

		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}
