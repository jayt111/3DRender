#include "Scene.h"

#include <algorithm>


void Scene::addRoot(SceneNode* node)
{
	roots.push_back(node);
}

void Scene::drawAll(Shader& shader, Shader& particleShader, const glm::mat4& view, const glm::mat4 projection, const glm::vec3 camPos)
{
	// Set shader
	shader.use();

	// Set uniforms
	// Matrix 4
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	// Vector 3
	shader.setVec3("camPos",camPos);

	// start drawing with identity matrix
	for (auto* root : roots) {
		if (!root->getIsTransparent())
			root->draw(glm::mat4(1.f), shader, particleShader, view, projection, false);
	}

	// draw transparent
	std::vector<std::pair<float, SceneNode*>> transparentNodes;
	for (auto* root : roots) {
		collectTransparentNodes(root, camPos, transparentNodes);
	}
	// Sort back to front
	std::sort(transparentNodes.begin(), transparentNodes.end(),
		[](const auto& a, const auto& b) {
			return a.first > b.first;
		});

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	for (auto& pair : transparentNodes) {
		pair.second->drawTransparent(shader, particleShader, view, projection);
	}
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// draw particles
	for (auto* root : roots) {
		root->draw(glm::mat4(1.f), shader, particleShader, view, projection, true);
	}
}

void Scene::drawAllShadow(Shader& shadowShader, const glm::mat4& projectedLightSpaceMatrix)
{
	shadowShader.use();
	shadowShader.setMat4("projectedLightSpaceMatrix", projectedLightSpaceMatrix);

	for (auto* root : roots) {
		root->drawShadow(glm::mat4(1.f), shadowShader);
	}
}

void Scene::collectTransparentNodes(SceneNode* node, const glm::vec3& camPos, std::vector<std::pair<float, SceneNode*>>& transparentNodes)
{
	// Traverse scene tree recursively and add transparent nodes
	if (node->getIsTransparent()) {
		float dist = glm::length(node->getWorldPosition() - camPos);
		transparentNodes.emplace_back(dist, node);
	}

	for (auto* child : node->getChildren()) {
		collectTransparentNodes(child, camPos, transparentNodes);
	}
}

void Scene::uploadLights(Shader& shader, std::vector<Light>& lights)
{
	shader.setInt("numLights", (int)lights.size());
	for (int i = 0; i < lights.size(); i++) {
		std::string prefix = "lights[" + std::to_string(i) + "]";
		shader.setVec3(prefix + ".position", lights[i].getPosition());
		shader.setVec3(prefix + ".direction", lights[i].getDirection());
		shader.setVec3(prefix + ".colour", lights[i].getColour());
		shader.setInt(prefix + ".type", (int)lights[i].getType());
	}
}

