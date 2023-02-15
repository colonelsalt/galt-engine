#pragma once

#include <glm/glm.hpp>

#include "Transform.h"
//#include "Renderer.h"

struct Primitive : public Component
{
	uint32_t VertexArrayId;
	int NumVertices;

	uint32_t DiffuseTextureId;
	uint32_t SpecularTextureId;
	uint32_t SkyboxTextureId;

	Shader* p_Shader;

	COMPONENT_DEF(ComponentType::PRIMITIVE, MAX_PRIMITIVES);

	void Draw(uint32_t shadowMapTextureId = 0);
};