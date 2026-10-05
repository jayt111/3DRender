#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "file.h"

class Shader
{
public:
	unsigned int ID;
	
	// Constructor
	Shader(const char* vsFilename, const char* fsFilename)
	{
		ID = CompileShader(vsFilename, fsFilename);
	}
	
	// Activate Shader
	void use() const
	{
		glUseProgram(ID);
	}
	
	// Fucntions for setting uniforms
	void setMat4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setVec4(const std::string& name, glm::vec4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

private:
	GLuint CompileShader(const char* vsFilename, const char* fsFilename) {
		int success;
		char infoLog[512];

		// Compile Vertex Shader
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		char* vertexShaderSource = read_file(vsFilename);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			fprintf(stderr, "Vertex Shader Compilation Fail - %s\n", infoLog);
		}

		// Compile Fragment Shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		char* fragmentShaderSource = read_file(fsFilename);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			fprintf(stderr, "Fragment Shader Compilation Fail - %s\n", infoLog);
		}

		// Link Program
		unsigned int program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			fprintf(stderr, "Shader Program Link Fail - %s\n", infoLog);
		}

		// Free Memory
		free(fragmentShaderSource);
		free(vertexShaderSource);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}
};