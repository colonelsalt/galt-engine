#pragma once

struct GenericComponentTable
{
	uint32_t IndexTable[MAX_ENTITIES];	
	uint32_t MaxComponents;
	uint32_t NumComponents;
};

template <typename T>
struct ComponentTable : public GenericComponentTable
{
	T* Components;

	inline void Init(uint32_t maxComponents)
	{
		MaxComponents = maxComponents;
		Components = (T*)g_Memory->TempAlloc(maxComponents * sizeof(T));
	}

	inline T* AddComponent(Entity entity)
	{
		Assert(Components && IndexTable[entity] == 0 && NumComponents < MaxComponents);

		IndexTable[entity] = NumComponents;
		T* component = &Components[NumComponents++];
		component->ThisEntity = entity;
		return component;
	}

	inline T* GetComponent(Entity entity)
	{
		Assert(Components && entity);
		uint32_t index = IndexTable[entity];
		Assert(index < MaxComponents);
		return &Components[index];
	}
	
};