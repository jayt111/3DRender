#include "Player.h"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <iostream>

Player::Player()
{
	InitCamera(camera);
	// Starting position of camera
	camera.Position = glm::vec3(0.f, 1.f, -5.f);
}

void Player::updateDirection(float xoffset, float yoffset)
{

	camera.Yaw += xoffset * camera.MouseSensitivity;
	camera.Pitch -= yoffset * camera.MouseSensitivity;

	// Clamp camera
	if (camera.Pitch > 89.0f)
		camera.Pitch = 89.0f;
	if (camera.Pitch < -89.f)
		camera.Pitch = -89.f;

	// Update Front based off new Yaw and Pitch
	glm::vec3 direction;
	direction.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	direction.y = sin(glm::radians(camera.Pitch));
	direction.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	camera.Front = glm::normalize(direction);

	camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
	camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
	}

void Player::move(SCamera::Camera_Movement dir, float dt)
{
	MoveCamera(camera, dir, dt);
}

glm::mat4 Player::getViewMatrix() const
{
	return glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
}

glm::vec3 Player::getPosition() const
{
	return camera.Position;
}
