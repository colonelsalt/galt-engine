#pragma once

#include <glm/glm.hpp>

#include "Input.h"

struct Camera
{
	glm::mat4 View;
	glm::mat4 Projection;

	float Fov;
	float Aspect;

	float Sensitivity;
	float MoveSpeed;

	glm::vec3 Position;
	glm::vec3 Forward; // Updated by mouse movement
	glm::vec3 Up;

	// Right direction is just cached to avoid needing to cross product every frame
	glm::vec3 Right;

	void Update(ControllerInput* input);
};