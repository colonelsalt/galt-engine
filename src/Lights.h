#pragma once

#include "Component.h"

enum class LightType
{
	POINT, DIRECTIONAL
};

struct Light : Component
{
	LightType Type;
	glm::vec3 Colour;
	union
	{
		glm::vec3 Position;
		glm::vec3 Direction;
	};

	COMPONENT_DEF(ComponentType::LIGHT, MAX_LIGHTS);

	// TODO: Generate light matrix
};