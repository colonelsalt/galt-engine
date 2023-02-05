#pragma once

#include <glm/glm.hpp>

struct Primitive
{
	uint32_t VertexArrayId;
	Shader* _Shader;
	int NumVertices;

	uint32_t TextureId;

	glm::mat4 Model;

	void Draw();
};