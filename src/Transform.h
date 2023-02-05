#pragma once

#include <glm/glm.hpp>

constexpr int MAX_TRANSFORM_CHILDREN = 3;

struct Transform
{
	glm::vec3 Translation;
	glm::vec3 EulerRotation;
	glm::vec3 Scale;

	// TODO
	Transform* Parent;
	Transform* Children[MAX_TRANSFORM_CHILDREN];

	inline static Transform Blank()
	{
		Transform result = {};
		result.Scale = { 1.0f, 1.0f, 1.0f };
		return result;
	}

	glm::mat4 ToModel();
};