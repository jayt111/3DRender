#include "SceneNode.h"

SceneNode::SceneNode(Model* model) 
	: model(model), particleEmitter(nullptr)
{
}

SceneNode::SceneNode(ParticleEmitter* emitter)
	:model(nullptr), particleEmitter(emitter)
{
}

void SceneNode::setPosition(const glm::vec3& pos)
{
	position = pos;
}

void SceneNode::setRotation(float angleDeg, const glm::vec3& axis)
{
	angle = angleDeg;
	rotationAxis = axis;
}

void SceneNode::setScale(const glm::vec3& s)
{
	scale = s;
}

void SceneNode::setIsTransparent(bool value)
{
	isTransparent = value;
}

void SceneNode::setParent(SceneNode* parentNode)
{
	parent = parentNode;
}

glm::vec3 SceneNode::getPosition()
{
	return position;
}

float SceneNode::getRotationAngle()
{
	return angle;
}

glm::vec3 SceneNode::getRotationAxis()
{
	return rotationAxis;
}

glm::vec3 SceneNode::getScale()
{
	return scale;
}

bool SceneNode::getIsTransparent()
{
	return isTransparent;
}

std::vector<SceneNode*> SceneNode::getChildren()
{
	return children;
}

glm::mat4 SceneNode::getLocalTransform() const
{
	glm::mat4 trans = glm::translate(glm::mat4(1.f), position);
	glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(angle), rotationAxis);
	glm::mat4 scl = glm::scale(glm::mat4(1.f), scale);
	return trans * rot * scl;
}

glm::mat4 SceneNode::getWorldTransform() const
{
	if (parent)
		return parent->getWorldTransform() * getLocalTransform();
	return getLocalTransform();
}

glm::vec3 SceneNode::getWorldPosition() const
{
	glm::mat4 world = getWorldTransform();
	return glm::vec3(world[3]);
}

void SceneNode::addChild(SceneNode* child)
{
	child->setParent(this);
	children.push_back(child);
}

void SceneNode::draw(const glm::mat4& parentTransform, Shader& shader, Shader& particleShader, const glm::mat4& view, const glm::mat4& projection, bool drawParticles) const
{
	// Apply transforms in local space
	glm::mat4 local = glm::mat4(1.f);
	local = glm::translate(local, position);
	local = glm::rotate(local, glm::radians(angle), rotationAxis);
	local = glm::scale(local, scale);
	
	// Convert to world space
	glm::mat4 worldTransform = parentTransform * local;
	
	// Draw self
	if (model && !drawParticles && !isTransparent) {
		shader.use();
		shader.setMat4("model", worldTransform);
		model->draw(shader);
	}

	if (particleEmitter && drawParticles) {
		glm::vec3 emitterWorldPos = glm::vec3(worldTransform[3]); // Get world pos
		particleEmitter->setEmitterPosition(emitterWorldPos);
		particleEmitter->draw(view, projection, particleShader);
	}
	
	// Draw children
	for (auto* child : children) {
		child->draw(worldTransform, shader, particleShader, view, projection, drawParticles);
	}
}

void SceneNode::drawShadow(const glm::mat4& parentTransform, Shader& shadowShader) const
{
	// Apply transforms in local space
	glm::mat4 local = glm::mat4(1.f);
	local = glm::translate(local, position);
	local = glm::rotate(local, glm::radians(angle), rotationAxis);
	local = glm::scale(local, scale);

	// Convert to world space
	glm::mat4 worldTransform = parentTransform * local;

	if (model && !isTransparent) {
		shadowShader.setMat4("model", worldTransform);
		model->drawShadow();
	}

	for (auto* child : children) {
		child->drawShadow(worldTransform, shadowShader);
	}
}

void SceneNode::drawTransparent(Shader& shader, Shader& particleShader, const glm::mat4& view, const glm::mat4& projection) const
{
	glm::mat4 worldTransform = getWorldTransform();

	if (model) {
		shader.use();
		shader.setMat4("model", worldTransform);
		model->draw(shader);
	}
}
