#include "Entity.h"

inline TransformComponent* Entity::AddTransform()
{
	return g_EntityMaster->AddTransform(*this);
}

inline MeshComponent* Entity::AddMesh()
{
	return g_EntityMaster->AddMesh(*this);
}

inline PrimitiveComponent* Entity::AddPrimitive()
{
	return g_EntityMaster->AddPrimitive(*this);
}

inline TransformComponent* Entity::GetTransform()
{
	return g_EntityMaster->GetTransform(*this);
}

inline MeshComponent* Entity::GetMesh()
{
	return g_EntityMaster->GetMesh(*this);
}

inline PrimitiveComponent* Entity::GetPrimitive()
{
	return g_EntityMaster->GetPrimitive(*this);
}