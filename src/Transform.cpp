#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.inl>

glm::mat4 Transform::ToModel()
{
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), Translation);
		
	glm::mat4 rot = glm::yawPitchRoll(EulerRotation.y,
	                                  EulerRotation.x,
	                                  EulerRotation.z);

	return trans * rot * glm::scale(glm::mat4(1.0f), Scale);
}