#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "shader.h"
#include "ParticleEmitter.h"

class SceneNode
{
public:
	// Constructor, if nothing passed in defaults to nullptr
	SceneNode(Model* model = nullptr);
	SceneNode(ParticleEmitter* emitter);
	
	// Setters
	void setPosition(const glm::vec3& pos);
	void setRotation(float angleDeg, const glm::vec3& axis);
	void setScale(const glm::vec3& s);
	void setIsTransparent(bool value);
	void setParent(SceneNode* parentNode);

	// Getters
	glm::vec3 getPosition();
	float getRotationAngle();
	glm::vec3 getRotationAxis();
	glm::vec3 getScale();
	bool getIsTransparent();
	std::vector<SceneNode*> getChildren();

	glm::mat4 getLocalTransform() const;
	glm::mat4 getWorldTransform() const;
	glm::vec3 getWorldPosition() const;
	
	void addChild(SceneNode* child);

	void draw(const glm::mat4& parentTransform, Shader& shader, Shader& particleShader, const glm::mat4& view, const glm::mat4& projection, bool drawParticles) const;

	void drawShadow(const glm::mat4& parentTransform, Shader& shadowShader) const;

	void drawTransparent(Shader& shader, Shader& particleShader, const glm::mat4& view, const glm::mat4& projection) const;

private:
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);
	float angle = 0.f;
	glm::vec3 rotationAxis = glm::vec3(0.f, 1.f, 0.f);

	SceneNode* parent = NULL;

	Model* model;
	ParticleEmitter* particleEmitter;
	std::vector<SceneNode*> children;
	bool isTransparent = false;
};

