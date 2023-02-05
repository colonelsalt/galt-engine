#pragma once

#include <glm/glm.hpp>

struct Primitive
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	glm::mat4 Model;
};