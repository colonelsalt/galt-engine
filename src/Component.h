#pragma once

#include "Entity.h"

constexpr uint32_t MAX_TRANSFORMS = MAX_ENTITIES;
constexpr uint32_t MAX_MESHES = 30;
constexpr uint32_t MAX_PRIMITIVES = 5;
constexpr uint32_t MAX_NAME_TAGS = MAX_ENTITIES;
constexpr uint32_t MAX_LIGHTS = 1;
constexpr uint32_t MAX_ANIMATORS = 1;
constexpr uint32_t MAX_PLAYERS = 1;

enum ComponentType
{
	INVALID = -1,
	TRANSFORM = 0, MESH = 1, PRIMITIVE = 2, NAME_TAG = 3, LIGHT = 4,
	ANIMATOR = 5, PLAYER = 6,
	NUM_COMPONENTS = 7
};

inline bool ValidComponentType(ComponentType type)
{
	return type != ComponentType::INVALID && type != ComponentType::NUM_COMPONENTS;
}

struct Transform;

struct Component
{
	Entity ThisEntity;

	Transform* Trans();

	template <typename T>
	T* GetComponent();

};

#define COMPONENT_DEF(type, maxComponents)\
inline static ComponentType GetType() { return (type); }\
inline static uint32_t GetMaxComponents() { return (maxComponents); }\
