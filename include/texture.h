#pragma once

#include <iostream>
#include "stb_image.h"

GLuint setup_texture(const char* filename)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Load Image
	int w, h, chan;
	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);
	std::cout << "Height: " << h << " Width: " << w << std::endl;
	if (pxls) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pxls);
	}
	else {
		std::cout << "Error Loading Image Texture" << std::endl;
	}
	delete[] pxls;
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return texObject;
}

GLuint setup_mipmaps(const char* filename[], int n)
{
	return 0;
}
