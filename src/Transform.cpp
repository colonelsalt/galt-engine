#include "Transform.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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

// Preserves original scale
void Transform::SetRotation(float radians, const glm::vec3& axis)
{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(Local, scale, rotation, translation, skew, perspective);

	Local = glm::translate(glm::mat4(1.0f), translation)
		* glm::rotate(glm::mat4(1.0f), radians, axis)
		* glm::scale(glm::mat4(1.0f), scale);
}