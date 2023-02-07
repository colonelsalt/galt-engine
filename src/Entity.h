#pragma once

#include "Transform.h"

enum RenderType
{
	NONE = 0, PRIMITIVE = 1, MESH = 2
};

struct Entity
{
	TransformComponent Transform;

	RenderType Type;
	union
	{
		MeshComponent Mesh;
		PrimitiveComponent Primitive;
	};

	void Draw(Shader* shader);
};