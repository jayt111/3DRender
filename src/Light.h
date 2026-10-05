#pragma once

#include <glm/glm.hpp>
#include <string>
#include "shadow.h"

enum class LightType {
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2
};

class Light {
public:
	Light(LightType type, const glm::vec3& colour, float z_near, float z_far, int width, int height);

	void setPosition(const glm::vec3& pos);
	void setDirection(const glm::vec3& dir);
	void setColour(const glm::vec3& col);
	void setShadowIndex(int index);
	void enableShadowMapping(int shadowWidth, int shadowHeight);

	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	glm::vec3 getColour() const;
	LightType getType() const;
	int getShadowIndex() const;
	const ShadowStruct& getShadowStruct() const;
	glm::mat4 getLightSpaceMatrix() const;

private:
	LightType type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 colour;
	int shadowIndex = -1;
	ShadowStruct shadow;
	float z_near, z_far;
	int width, height;
};
