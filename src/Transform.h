#pragma once

#include <glm/glm.hpp>

#include "Component.h"

struct TransformComponent : public Component
{
	glm::mat4 Model;

	TransformComponent* p_Parent;
	TransformComponent** a_Children;
	
	// Is storing this value even necessary? Will it ever be used outside
	// of allocation?
	uint32_t NumChildren;

	void CompInit(uint32_t numChildren = 0);

	inline glm::mat4 WorldSpace()
	{
		if (!p_Parent)
		{
			return Model;
		}
		return p_Parent->WorldSpace() * Model;
	}

	inline glm::vec3* Translation()
	{
		return (glm::vec3*)&Model[3];
	}

	// Is this right??
	inline const glm::vec3 Forward()
	{
		return *(glm::vec3*)&Model[2];
	}

	inline const glm::vec3 GetScale()
	{
		return { Model[0][0], Model[1][1], Model[2][2] };
	}

	inline void SetScale(const glm::vec3& scale)
	{
		Model[0][0] = scale.x;
		Model[1][1] = scale.y;
		Model[2][2] = scale.z;
	}

	void SetRotation(float yaw, float pitch, float roll);

};