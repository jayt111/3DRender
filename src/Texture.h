#pragma once

#include <GL/gl3w.h>
#include <vector>
#include <string>

GLuint LoadTexture(const char* path);

GLuint LoadCubemap(std::vector<std::string> faces);

