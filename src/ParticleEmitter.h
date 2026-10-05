#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime = 0; // Seconds
	float maxLifetime;
	float size;
	int tileIndex;
	bool isSmoke;
	float cameraDistance = -1.f;
};

class ParticleEmitter {
public:
	ParticleEmitter(unsigned int maxParticles, const std::string& fireTexturePath, const std::string& smokeTexturePath);

	void update(float dt);
	void draw(const glm::mat4& view, const glm::mat4& projection, Shader& shader);

	void setEmitterPosition(const glm::vec3& pos);
	void setWind(const glm::vec3& dir, float strength);


private:
	void respawnParticle(Particle& particle);
	void setup();

	unsigned int maxParticles;
	std::vector<Particle> particles;

	glm::vec3 windDirection;
	float windStrength = 0.f;

	GLuint vao = 0, vbo = 0;
	GLuint instanceVBO = 0, tileIndexVBO = 0, lifetimeVBO = 0, smokeVBO = 0;

	GLuint fireTextureID = 0, smokeTextureID = 0;

	int framesX = 3;
	int framesY = 2;
	
	bool posSet = false;

	glm::vec3 emitterPosition = glm::vec3(0);
};

