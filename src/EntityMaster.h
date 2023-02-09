#pragma once

#include "Transform.h"

// Technically this is MAX_ENTITIES-1, since 0 is not a valid entityId
constexpr uint32_t MAX_ENTITIES = 50;

constexpr uint32_t MAX_TRANSFORMS = 50;
constexpr uint32_t MAX_MESHES = 30;
constexpr uint32_t MAX_PRIMITIVES = 5;

enum ComponentType
{
	TRANSFORM = 0, MESH = 1, PRIMITIVE = 2
};

struct EntityMaster
{
	uint32_t NumEntities;

	uint32_t NumTransforms;
	uint32_t TransformIndexTable[MAX_ENTITIES];
	TransformComponent TransformTable[MAX_TRANSFORMS];

	uint32_t NumMeshes;
	uint32_t MeshIndexTable[MAX_ENTITIES];
	MeshComponent MeshTable[MAX_MESHES];
	
	uint32_t NumPrimitives;
	uint32_t PrimitiveIndexTable[MAX_ENTITIES];
	PrimitiveComponent PrimitivesTable[MAX_PRIMITIVES];

	// Entity has component c if bit (1 << c) is set
	uint32_t ComponentBitSet[MAX_ENTITIES];

	inline Entity CreateEntity()
	{
		NumEntities++;
		Assert(NumEntities < MAX_ENTITIES);
		return { NumEntities };
	}

	// ---
	// Adding components ---
	// ---
	inline TransformComponent* AddTransform(Entity entity)
	{
		Assert(entity && TransformIndexTable[entity] == 0);
		TransformIndexTable[entity] = NumTransforms;
		ComponentBitSet[entity] |= (1 << ComponentType::TRANSFORM);

		TransformComponent* transform = &TransformTable[NumTransforms++];
		transform->EntityId = entity;
		return transform;
	}

	inline MeshComponent* AddMesh(Entity entity)
	{
		Assert(entity && MeshIndexTable[entity] == 0);
		MeshIndexTable[entity] = NumMeshes;
		ComponentBitSet[entity] |= (1 << ComponentType::MESH);

		MeshComponent* mesh = &MeshTable[NumMeshes++];
		mesh->EntityId = entity;
		return mesh;
	}

	inline PrimitiveComponent* AddPrimitive(Entity entity)
	{
		Assert(entity && PrimitiveIndexTable[entity] == 0);
		PrimitiveIndexTable[entity] = NumPrimitives;
		ComponentBitSet[entity] |= (1 << ComponentType::PRIMITIVE);

		PrimitiveComponent* primitive = &PrimitivesTable[NumPrimitives++];
		primitive->EntityId = entity;
		return primitive;
	}

	// ---
	// Getting components ---
	// ---
	inline TransformComponent* GetTransform(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
		if (ComponentBitSet[entity] & (1 << ComponentType::TRANSFORM))
		{
			uint32_t index = TransformIndexTable[entity];
			Assert(index < MAX_TRANSFORMS);
			return &TransformTable[index];
		}
		return nullptr;
	}

	inline MeshComponent* GetMesh(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
		if (ComponentBitSet[entity] & (1 << ComponentType::MESH))
		{
			uint32_t index = MeshIndexTable[entity];
			Assert(index < MAX_MESHES);
			return &MeshTable[index];
		}
		return nullptr;
	}

	inline PrimitiveComponent* GetPrimitive(Entity entity)
	{
		Assert(entity && entity < MAX_ENTITIES);
		if (ComponentBitSet[entity] & (1 << ComponentType::PRIMITIVE))
		{
			uint32_t index = PrimitiveIndexTable[entity];
			Assert(index < MAX_PRIMITIVES);
			return &PrimitivesTable[index];
		}
		return nullptr;
	}
};