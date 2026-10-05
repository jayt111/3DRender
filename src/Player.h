#pragma once
// FPS style camera movement clas

#include "camera.h"

class Player
{
public:
	SCamera camera;

	Player();

	void updateDirection(float xoffset, float yoffset);

	void move(SCamera::Camera_Movement dir, float dt);

	glm::mat4 getViewMatrix() const;

	glm::vec3 getPosition() const;
};

