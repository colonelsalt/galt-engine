#include "EntityMaster.h"

#include "NameTag.h"

template <typename T>
static void InitComponentType()
{
	ComponentTable<T>* table = (ComponentTable<T>*)
		g_Memory->TempAlloc(sizeof(ComponentTable<T>));
	table->Init(T::GetMaxComponents());
	g_EntityMaster->ComponentTables[T::GetType()] = table;
}

void EntityMaster::InitComponents()
{
	InitComponentType<Transform>();
	InitComponentType<Mesh>();
	InitComponentType<Primitive>();
	InitComponentType<NameTag>();
}