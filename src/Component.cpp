#include "Component.h"

Transform* Component::Trans()
{
	return g_EntityMaster->GetComponent<Transform>(ThisEntity);
}

template <typename T>
T* Component::GetComponent()
{
	return g_EntityMaster->GetComponent<T>(ThisEntity);
}