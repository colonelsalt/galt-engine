#pragma once

#include <glm/glm.hpp>

struct Transform
{
	glm::vec3 Translation;
	glm::vec3 EulerRotation;
	glm::vec3 Scale;

	// TODO
	Transform* Parent;
	Transform** Children;
	uint32_t NumChildren;

	inline static Transform Blank()
	{
		Transform result = {};
		result.Scale = { 1.0f, 1.0f, 1.0f };
		return result;
	}

	glm::mat4 ToModel();
};