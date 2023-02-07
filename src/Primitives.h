#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

struct PrimitiveComponent
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	uint32_t TextureId;

	// TODO: This seems a little hacky - consider doing full ECS later
	Entity* p_Entity;

	void Draw();
};