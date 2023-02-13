#include "Transform.h"

#include <glm/gtx/euler_angles.hpp>

void Transform::Init()
{
	Local = glm::mat4(1.0f);
}

void Transform::AllocChildren(uint32_t numChildren)
{
	Assert(NumChildren == 0 && !a_Children); // Cannot allocate children twice

	NumChildren = numChildren;
	if (NumChildren)
	{
		a_Children = (Transform**)
			g_Memory->TempAlloc(NumChildren * sizeof(Transform*));
	}

}

void Transform::SetRotation(float yaw, float pitch, float roll)
{
	glm::vec3* translation = Position();
	glm::vec3 scale = GetScale();

	glm::mat4 rotation = glm::yawPitchRoll(glm::radians(yaw),
	                                       glm::radians(pitch),
	                                       glm::radians(roll));

	// TODO: Fix retaining original scale here

	//Model = rotation;
	Local = glm::translate(glm::mat4(1.0f), *translation);
	Local *= rotation;
	//Model *= glm::scale(glm::mat4(1.0f), scale);
}