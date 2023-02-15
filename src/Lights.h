#pragma once

#include "Component.h"

enum class LightType
{
	POINT, DIRECTIONAL
};

struct LightMatrix
{
	glm::mat4 Projection;
	glm::mat4 View;
};

struct Light : Component
{
	LightType Type;
	glm::vec3 Colour;

	COMPONENT_DEF(ComponentType::LIGHT, MAX_LIGHTS);

	LightMatrix GetLightMatrix(const glm::vec3* targetPos);
};