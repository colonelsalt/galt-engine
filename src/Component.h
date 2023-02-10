#pragma once

#include "Entity.h"

constexpr uint32_t MAX_TRANSFORMS = MAX_ENTITIES;
constexpr uint32_t MAX_MESHES = 30;
constexpr uint32_t MAX_PRIMITIVES = 5;
constexpr uint32_t MAX_NAME_TAGS = MAX_ENTITIES;

enum ComponentType
{
	INVALID = -1,
	TRANSFORM = 0, MESH = 1, PRIMITIVE = 2, NAME_TAG = 3,
	NUM_COMPONENTS = 4
};

inline bool ValidComponentType(ComponentType type)
{
	return type != ComponentType::INVALID && type != ComponentType::NUM_COMPONENTS;
}

struct Component
{
	uint32_t EntityId;
};

#define COMPONENT_DEF(type, maxComponents)\
inline static ComponentType GetType() { return (type); }\
inline static uint32_t GetMaxComponents() { return (maxComponents); }\
