#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

struct Primitive : public Component
{
	uint32_t VertexArrayId;
	int NumVertices;

	uint32_t DiffuseTextureId;
	uint32_t SpecularTextureId;

	Shader* p_Shader;

	COMPONENT_DEF(ComponentType::PRIMITIVE, MAX_PRIMITIVES);

	void Draw();
};