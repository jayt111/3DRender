#pragma once

#include <glm/glm.hpp>

class DayNightCycle
{
public:
	void update(float dt);
	glm::vec3 getColour();
	glm::vec3 getPosition();
	glm::vec3 getDirection();

	glm::vec3 getBackgroundColour();

private:
	glm::vec3 colour = glm::vec3(0.f);
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 direction = glm::vec3(0.f);

	glm::vec3 backgroundColour;

	glm::vec3 nightColour = glm::vec3(0.05f, 0.05f, 0.2f);
	glm::vec3 dawnColour = glm::vec3(1.f, 0.5f, 0.2f);
	glm::vec3 dayColour = glm::vec3(1.f, 1.f, 0.9f);
	
	glm::vec3 nightColorBackground = glm::vec3(0.02f, 0.02f, 0.05f);       // dark blue-black
	glm::vec3 sunriseColorBackgroiund = glm::vec3(0.8f, 0.4f, 0.2f);         // orange-pink
	glm::vec3 dayColorBackground = glm::vec3(0.5f, 0.7f, 1.0f);             // bright blue

	float dayDuration = 60.f; // 1 minute
	float currentTime = 0.f;
};

