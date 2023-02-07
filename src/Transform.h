#pragma once

#include <glm/glm.hpp>

struct Entity;

struct TransformComponent
{
	glm::mat4 Model;

	TransformComponent* p_Parent;
	TransformComponent** a_Children;
	uint32_t NumChildren;

	// TODO: This seems a little hacky - consider doing full ECS later
	Entity* p_Entity;

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

	inline static TransformComponent Identity()
	{
		return { glm::mat4(1.0f) };
	}

	void SetRotation(float yaw, float pitch, float roll);

};