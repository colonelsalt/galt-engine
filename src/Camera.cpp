#include "Camera.h"

static Camera CreateCamera()
{
	Camera result = {};

	result.Position = { 0.0f, 0.0f, 3.0f };
	result.Forward = { 0.0f, 0.0f, -1.0f };
	result.Up = { 0.0f, 1.0f, 0.0f };
	result.Right = { 1.0f, 0.0f, 0.0f };

	result.Sensitivity = 0.1f;
	result.MoveSpeed = 2.5f;

	result.Fov = glm::pi<float>() / 4.0f;
	result.Aspect = 16.0f / 9.0f;

	result.View = glm::translate(glm::mat4(1.0f), -result.Position);
	result.Projection = glm::perspective(result.Fov, result.Aspect, 0.1f, 100.0f);

	return result;
}

void Camera::Update(ControllerInput* input)
{
	float speed = MoveSpeed * input->DeltaTime;

	if (input->Up)
	{
		Position += speed * Forward;
	}
	if (input->Down)
	{
		Position -= speed * Forward;
	}
	if (input->Left)
	{
		Position -= speed * Right;
	}
	if (input->Right)
	{
		Position += speed * Right;
	}
	if (input->LeftShoulder)
	{
		Position += speed * Up;
	}
	if (input->RightShoulder)
	{
		Position -= speed * Up;
	}

	View = glm::translate(glm::mat4(1.0f), -Position);
}