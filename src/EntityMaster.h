#pragma once

#include "ComponentTable.h"
#include "HashMap.h"

struct EntityMaster
{
	uint32_t NumEntities;

	GenericComponentTable* ComponentTables[NUM_COMPONENTS];

	// Entity has component c if bit (1 << c) is set
	uint32_t ComponentBitSet[MAX_ENTITIES];
	HashMap EntityNameMap;

	void InitComponents();

	Entity CreateEntity(char* name);

	inline Entity GetEntityByName(char* name)
	{
		int entityId = EntityNameMap.Get(name);
		if (entityId == -1)
		{
			return { 0 };
		}
		return { (uint32_t)entityId };
	}

	inline Entity GetOrCreateEntity(char* name)
	{
		Entity entity = GetEntityByName(name);
		if (entity)
		{
			return entity;
		}
		return CreateEntity(name);
	}

	template <typename T>
	inline bool HasComponent(Entity entity)
	{
		return ComponentBitSet[entity] & (1 << T::GetType());
	}

	template <typename T>
	inline T* AddComponent(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
		Assert(!HasComponent<T>(entity));

		ComponentBitSet[entity] |= (1 << T::GetType());
		ComponentTable<T>* table = (ComponentTable<T>*)
			ComponentTables[T::GetType()];
		Assert(table);
		return table->AddComponent(entity);
	}

	template <typename T>
	inline T* GetComponent(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
		if (HasComponent<T>(entity))
		{
			ComponentTable<T>* table = (ComponentTable<T>*)
				ComponentTables[T::GetType()];
			Assert(table);
			return table->GetComponent(entity);
		}
		return nullptr;
	}

	template <typename T>
	inline T* GetAllComponents()
	{
		ComponentTable<T>* table = (ComponentTable<T>*)ComponentTables[T::GetType()];
		return table->Components;
	}

	inline uint32_t NumComponents(ComponentType type)
	{
		return ComponentTables[type]->NumComponents;
	}
};