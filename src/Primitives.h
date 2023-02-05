#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

struct Primitive
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	uint32_t TextureId;

	Transform Trans;

	void Draw();
};