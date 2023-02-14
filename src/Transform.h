#pragma once

#include <glm/glm.hpp>

#include "Component.h"

struct Transform : public Component
{
	glm::mat4 Local;
	glm::mat4 Global;

	Transform* p_Parent;
	Transform** a_Children;
	
	// Is storing this value even necessary? Will it ever be used outside
	// of allocation?
	uint32_t NumChildren;

	COMPONENT_DEF(ComponentType::TRANSFORM, MAX_TRANSFORMS);

	void Init();

	void AllocChildren(uint32_t numChildren);

	inline void Update(glm::mat4 parentTransform = glm::mat4(1.0f))
	{
		//Global = Local;
		//return;

		Global = parentTransform * Local;
		for (uint32_t i = 0; i < NumChildren; i++)
		{
			Transform* child = a_Children[i];
			Assert(child);
			child->Update(Global);
		}
	}

	inline glm::vec3* Position()
	{
		return (glm::vec3*)&Local[3];
	}

	// Is this right??
	inline const glm::vec3 Forward()
	{
		return *(glm::vec3*)&Local[2];
	}

	inline const glm::vec3& Up()
	{
		return *(glm::vec3*)&Local[1];
	}

	inline const glm::vec3 GetScale()
	{
		return { Local[0][0], Local[1][1], Local[2][2] };
	}

	inline void SetScale(const glm::vec3& scale)
	{
		Local[0][0] = scale.x;
		Local[1][1] = scale.y;
		Local[2][2] = scale.z;
	}

	void LookDir(const glm::vec3& forward);

	void SetRotation(float yaw, float pitch, float roll);

	void SetRotation(float degrees, const glm::vec3& axis);

};