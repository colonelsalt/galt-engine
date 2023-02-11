#include "EntityMaster.h"

#include "NameTag.h"
#include "Lights.h"

template <typename T>
static void InitComponentType()
{
	ComponentTable<T>* table = (ComponentTable<T>*)
		g_Memory->TempAlloc(sizeof(ComponentTable<T>));
	table->Init(T::GetMaxComponents());
	g_EntityMaster->ComponentTables[T::GetType()] = table;
}

Entity EntityMaster::CreateEntity(char* name)
{
	NumEntities++;
	Assert(NumEntities < MAX_ENTITIES);
	Entity entity = { NumEntities };
	NameTag* nameTag = AddComponent<NameTag>(entity);
	nameTag->Init(name);

	Transform* transform = AddComponent<Transform>(entity);
	transform->CompInit();

	return entity;
}

void EntityMaster::InitComponents()
{
	InitComponentType<Transform>();
	InitComponentType<Mesh>();
	InitComponentType<Primitive>();
	InitComponentType<NameTag>();
	InitComponentType<Light>();
}