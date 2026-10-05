#include "camera.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// Moved function definitions here to stop linking errors

void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);

	in.Yaw = -90.f;
	in.Pitch = 0.f;

	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

}

float cam_dist = 2.f;

void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{
	// apply offset to pitch and yaw
	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	// Limit
	if (in.Pitch > 89.f) in.Pitch = 89.f;
	if (in.Pitch < -89.f) in.Pitch = -89.f;

	// Calculate new position
	in.Position.x = cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
	in.Position.y = sin(glm::radians(in.Pitch));
	in.Position.z = sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
	in.Position *= distance;
	// Calculate new Front direction
	in.Front = glm::normalize(target - in.Position);
	// Calculate new Right direction
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	// Calculate new Up direction
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
}

// Added in new function for fps like movement
void MoveCamera(SCamera& in, SCamera::Camera_Movement direction, float deltaTime)
{
	float velocity = in.MovementSpeed * deltaTime;

	if (direction == SCamera::FORWARD)
		in.Position += in.Front * velocity;

	if (direction == SCamera::BACKWARD)
		in.Position -= in.Front * velocity;
	
	if (direction == SCamera::LEFT)
		in.Position -= in.Right * velocity;

	if (direction == SCamera::RIGHT)
		in.Position += in.Right * velocity;

	if (direction == SCamera::UP)
		in.Position += in.WorldUp * velocity;

	if (direction == SCamera::DOWN)
		in.Position -= in.WorldUp * velocity;
}