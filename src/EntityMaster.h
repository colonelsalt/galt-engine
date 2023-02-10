#pragma once

#include "ComponentTable.h"

struct EntityMaster
{
	uint32_t NumEntities;

	GenericComponentTable* ComponentTables[NUM_COMPONENTS];

	// Entity has component c if bit (1 << c) is set
	uint32_t ComponentBitSet[MAX_ENTITIES];

	void InitComponents();

	inline Entity CreateEntity()
	{
		NumEntities++;
		Assert(NumEntities < MAX_ENTITIES);
		return { NumEntities };
	}

	template <typename T>
	inline T* AddComponent(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
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
		if (ComponentBitSet[entity] & (1 << T::GetType()))
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