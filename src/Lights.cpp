#include "Lights.h"

LightMatrix Light::GetLightMatrix(const glm::vec3* targetPos)
{
	// TODO: Support other light types here
	Assert(Type == LightType::POINT);

	LightMatrix result = {};
	result.Projection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, 1.0f, 35.0f);

	// Point towards the ground at the midway point between the light and the target
	glm::vec3 position = *(Trans()->Position());

	glm::vec3 lightGround = position;
	lightGround.y = 0.0f;
	glm::vec3 targetGround = *targetPos;
	targetGround.y = 0.0f;

	glm::vec3 targetPoint = lightGround + ((targetGround - lightGround) / 2.0f);
	result.View = glm::lookAt(position, targetPoint, { 0.0f, 1.0f, 0.0f });
	return result;
}