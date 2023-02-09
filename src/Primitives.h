#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

struct PrimitiveComponent : public Component
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	uint32_t TextureId;

	void Draw();
};