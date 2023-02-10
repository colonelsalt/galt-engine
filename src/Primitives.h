#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

struct Primitive : public Component
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	uint32_t TextureId;

	COMPONENT_DEF(ComponentType::PRIMITIVE, MAX_PRIMITIVES);

	void Draw();
};