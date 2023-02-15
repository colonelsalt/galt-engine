#include "Entity.h"


char* Entity::Name()
{
	NameTag* nameTag = GetComponent<NameTag>();
	Assert(nameTag);
	return nameTag->Tag;
}

template <typename T>
T* Entity::AddComponent()
{
	return g_EntityMaster->AddComponent<T>(*this);
}

template <typename T>
T* Entity::GetComponent()
{
	return g_EntityMaster->GetComponent<T>(*this);
}