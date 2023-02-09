#pragma once

struct Entity
{
	uint32_t Id;

	operator uint32_t() const
	{
		return Id;
	}

	TransformComponent* AddTransform();

	MeshComponent* AddMesh();

	PrimitiveComponent* AddPrimitive();

	TransformComponent* GetTransform();

	MeshComponent* GetMesh();

	PrimitiveComponent* GetPrimitive();

};