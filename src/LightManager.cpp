#include "LightManager.h"

void LightManager::addLight(const Light& light)
{
	// Add light to vector
	lights.push_back(light);
	// set shadowIndex
	if (light.getShadowIndex() == -1 &&
		(light.getType() == LightType::DIRECTIONAL || light.getType() == LightType::SPOT)) {
		lights.back().setShadowIndex((int)(lights.size() - 1));
		std::cout << "Shadow Index: " << (int)(lights.size() - 1) << " Light Type: " << (int)light.getType() << std::endl;
	}
}

void LightManager::updateLight(int index, const glm::vec3& pos, const glm::vec3& dir)
{
	if (index >= 0 && index < lights.size()) {
		lights[index].setPosition(pos);
		lights[index].setDirection(dir);
	}
}

void LightManager::updateLightColour(int index, const glm::vec3& col)
{
	if (index >= 0 && index < lights.size()) {
		lights[index].setColour(col);
	}
}

void LightManager::uploadLightsToShader(Shader& shader)
{
	shader.use();
	shader.setInt("numLights", (int)lights.size());
	for (int i = 0; i < lights.size(); i++) {
		const auto& light = lights[i];
		std::string prefix = "lights[" + std::to_string(i) + "]";

		shader.setVec3(prefix + ".position", light.getPosition());
		shader.setVec3(prefix + ".direction", light.getDirection());
		shader.setVec3(prefix + ".colour", light.getColour());
		shader.setInt(prefix + ".type", (int)light.getType());
		shader.setInt(prefix + ".shadowIndex", light.getShadowIndex());
	}
}

void LightManager::bindShadowMaps(Shader& shader, GLuint startingTextureUnit) const
{
	shader.use();
	GLuint texUnit = startingTextureUnit;
	for (const auto& light : lights) {
		int shadowIdx = light.getShadowIndex();
		if (shadowIdx >= 0) {
			glActiveTexture(GL_TEXTURE0 + texUnit);
			glBindTexture(GL_TEXTURE_2D, light.getShadowStruct().Texture);
			shader.setInt("shadowMaps[" + std::to_string(shadowIdx) + "]", texUnit);
			shader.setMat4("lightSpaceMatrices[" + std::to_string(shadowIdx) + "]", light.getLightSpaceMatrix());
			texUnit++;
		}
	}
}

void LightManager::generateShadowMaps(Scene& scene, Shader& shadowShader, int width, int height) const
{
	for (const auto& light : lights) {
		if (light.getShadowIndex() >= 0) {
			const auto& shadow = light.getShadowStruct();
			glm::mat4 lightMatrix = light.getLightSpaceMatrix();

			glViewport(0, 0, width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			scene.drawAllShadow(shadowShader, lightMatrix);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}

const std::vector<Light>& LightManager::getLights() const
{
	return lights;
}

Light& LightManager::getLight(int index)
{
	return lights[index];
}
