/*
* ParticleEmitter.cpp 
* 
* This is a particle emitter class specifically for a campfire
* It has both smoke and fire particles using instancing of a single Quad for performance to reduce draw calls
* The particles can be affected by wind
* Uses a Texture atlas for fire and smoke
* 
*/

#include "ParticleEmitter.h"
#include "Texture.h"
#include <algorithm>


ParticleEmitter::ParticleEmitter(unsigned int maxParticles, const std::string& fireTexturePath, const std::string& smokeTexturePath)
	: maxParticles(maxParticles)
{
	particles.resize(maxParticles);
	fireTextureID = LoadTexture(fireTexturePath.c_str());
	smokeTextureID = LoadTexture(smokeTexturePath.c_str());
	setWind(glm::vec3(1, 0, 0), 1.f);
	setup();
}

void ParticleEmitter::update(float dt)
{
	if (!posSet)
		return;
	for (auto& particle : particles) {
		particle.lifetime -= dt;
		if (particle.lifetime <= 0.0f) {
			respawnParticle(particle);
		}
		else {
			// apply wind
			glm::vec3 wind = windDirection;
			if (particle.isSmoke) {
				wind = wind * windStrength; // smoke fully affected
			}
			else {
				wind = wind * (windStrength * 0.5f); // fire affected half as much
			}
			
			particle.velocity += wind * dt;
			// Move particle
			particle.position += particle.velocity * dt;
		}
	}
}

void ParticleEmitter::draw(const glm::mat4& view, const glm::mat4& projection, Shader& shader)
{
	if (!posSet)
		return;
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glm::vec3 cameraPos = glm::vec3(glm::inverse(view)[3]);
	for (auto& p : particles) {
		p.cameraDistance = glm::length(p.position - cameraPos);
	}

	// sort back to front
	std::sort(particles.begin(), particles.end(),
		[](const Particle& a, const Particle& b) {
			return a.cameraDistance > b.cameraDistance;
		});

	std::vector<glm::vec4> instanceData; // pos + size
	std::vector<int> tileIndices;
	std::vector<float> normalisedLifetimes;
	std::vector<GLint> isSmokes;
	for (auto& p : particles) {
		if (p.lifetime > 0.0f) {
			instanceData.emplace_back(glm::vec4(p.position, p.size));
			tileIndices.push_back(p.tileIndex);
			normalisedLifetimes.push_back(p.lifetime / p.maxLifetime);
			isSmokes.push_back(p.isSmoke ? 1 : 0);
		}
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glEnable(GL_DEPTH_TEST);

	// upload instance data (pos + size)
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(glm::vec4), instanceData.data());

	// tileIndex
	glBindBuffer(GL_ARRAY_BUFFER, tileIndexVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tileIndices.size() * sizeof(int), tileIndices.data());

	// normalisedLifetimes
	glBindBuffer(GL_ARRAY_BUFFER, lifetimeVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, normalisedLifetimes.size() * sizeof(float), normalisedLifetimes.data());

	// isSmoke
	glBindBuffer(GL_ARRAY_BUFFER, smokeVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, isSmokes.size() * sizeof(int), isSmokes.data());

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fireTextureID);
	shader.setInt("particleTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, smokeTextureID);
	shader.setInt("smokeTexture", 1);

	glBindVertexArray(vao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, instanceData.size());
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

}

void ParticleEmitter::setEmitterPosition(const glm::vec3& pos)
{
	emitterPosition = pos;
	posSet = true;
}

void ParticleEmitter::setWind(const glm::vec3& dir, float strength)
{
	windDirection = glm::normalize(dir);
	windStrength = strength;
}

void ParticleEmitter::respawnParticle(Particle& particle)
{
	float randXPos = (float)(rand()) / (float)(RAND_MAX);
	float randYPos = (float)(rand()) / (float)(RAND_MAX);
	float randZPos = (float)(rand()) / (float)(RAND_MAX);

	particle.position = emitterPosition + glm::vec3(randXPos, randYPos, randZPos); 

	particle.isSmoke = (rand() % 500 == 0);
	if (particle.isSmoke) {
		// Slower velocity for smoke
		particle.velocity = glm::vec3(
			(rand() % 100 - 50) / 300.f,   
			1.0f + (rand() % 100) / 300.f,  
			(rand() % 100 - 50) / 300.f 
		);
		particle.lifetime = 4.0f + (rand() % 200) / 100.f; // Longer lifetime for smoke
		particle.size = 0.5f + (rand() % 100) / 500.f;
	}
	else {
		particle.velocity = glm::vec3(
			(rand() % 200 - 100) / 300.f,
			1.0f + (rand() % 100) / 200.f,
			(rand() % 200 - 100) / 300.f
		);
		particle.lifetime = 0.5f + (rand() % 100) / 50.f;
		particle.size = 0.2f + (rand() % 100) / 500.f;
	}
	particle.maxLifetime = particle.lifetime;

	particle.tileIndex = rand() % (framesX * framesY);
}

void ParticleEmitter::setup()
{
	// Quad vertices (will be billboarded so XY only)
	float quadVertices[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f
	};

	// Setup quad VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Quad VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Setup instace vbo 
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	// Layout: vec3 position + float size packed into vec4
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1); // advance 1 per instance
	
	// tileIndex (int)
	glGenBuffers(1, &tileIndexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tileIndexVBO);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(int), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	// normalised lifetime
	glGenBuffers(1, &lifetimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lifetimeVBO);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	// isSmoke
	glGenBuffers(1, &smokeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, smokeVBO);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(4, 1, GL_INT, sizeof(int), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);


	glBindVertexArray(0);
}
