#pragma once

// Split into header an cpp file to stop linking errors

#include <stdio.h>
#include <glm/glm.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = 10.f;
	float MouseSensitivity = 50.0f;



};

void InitCamera(SCamera& in);
extern float cam_dist;

void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset);

// Added in new function for fps like movement
void MoveCamera(SCamera& in, SCamera::Camera_Movement direction, float deltaTime);