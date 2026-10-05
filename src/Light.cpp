#include "Light.h"

#include <glm/gtc/matrix_transform.hpp>

Light::Light(LightType type, const glm::vec3& colour, float z_near, float z_far, int width, int height) :
	type(type),
	colour(colour),
	z_near(z_near),
	z_far(z_far),
	position(glm::vec3(0.f)),
	direction(glm::vec3(0.f, -1.f, 0.f)),
	width(width),
	height(height)
{
}

void Light::setPosition(const glm::vec3& pos)
{
	position = pos;
}

void Light::setDirection(const glm::vec3& dir)
{
	direction = dir;
}

void Light::setColour(const glm::vec3& col)
{
	colour = col;
}

void Light::setShadowIndex(int index)
{
	shadowIndex = index;
}

void Light::enableShadowMapping(int shadowWidth, int shadowHeight)
{
	shadow = setup_shadowmap(shadowWidth, shadowHeight);
}

glm::vec3 Light::getPosition() const
{
	return position;
}

glm::vec3 Light::getDirection() const
{
	return direction;
}

glm::vec3 Light::getColour() const
{
	return colour;
}

LightType Light::getType() const
{
	return type;
}

int Light::getShadowIndex() const
{
	return shadowIndex;
}

const ShadowStruct& Light::getShadowStruct() const
{
	return shadow;
}

glm::mat4 Light::getLightSpaceMatrix() const
{
	if (type == LightType::DIRECTIONAL) {
		glm::mat4 lightProjection = glm::ortho(-20.f, 20.f, -20.f, 20.f, z_near, z_far);
		glm::mat4 lightView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
		return lightProjection * lightView;
	}else if (type == LightType::SPOT) {
		glm::mat4 lightProjection = glm::perspective(glm::radians(25.f), (float)width / (float)height, z_near, 100.f);
		glm::mat4 lightView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
		return lightProjection * lightView;
	}
	return glm::mat4(1.f);
}
