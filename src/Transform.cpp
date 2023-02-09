#include "Transform.h"

#include <glm/gtx/euler_angles.hpp>

void TransformComponent::CompInit(uint32_t numChildren)
{
	Model = glm::mat4(1.0f);
	NumChildren = numChildren;
	if (NumChildren)
	{
		a_Children = (TransformComponent**)
			g_Memory->TempAlloc(NumChildren * sizeof(TransformComponent*));
	}
}

void TransformComponent::SetRotation(float yaw, float pitch, float roll)
{
	glm::vec3* translation = Translation();
	glm::vec3 scale = GetScale();

	glm::mat4 rotation = glm::yawPitchRoll(glm::radians(yaw),
	                                       glm::radians(pitch),
	                                       glm::radians(roll));

	// TODO: Fix retaining original scale here

	//Model = rotation;
	Model = glm::translate(glm::mat4(1.0f), *translation);
	Model *= rotation;
	//Model *= glm::scale(glm::mat4(1.0f), scale);
}