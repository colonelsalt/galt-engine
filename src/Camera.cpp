#include "Camera.h"

static Camera CreateCamera()
{
	Camera result = {};

	result.Position = { 0.0f, 0.0f, 3.0f };
	result.Forward = { 0.0f, 0.0f, -1.0f };
	result.Up = { 0.0f, 1.0f, 0.0f };
	result.Right = { 1.0f, 0.0f, 0.0f };

	result.MouseSensitivity = 0.001f;
	result.StickSensitivity = 2.0f / DELTA_TIME_SCALE;
	result.MoveSpeed = 3.5f / DELTA_TIME_SCALE;

	result.Fov = glm::pi<float>() / 4.0f;
	result.Aspect = 16.0f / 9.0f;

	result.View = glm::translate(glm::mat4(1.0f), -result.Position);
	result.Projection = glm::perspective(result.Fov, result.Aspect, 0.1f, 100.0f);

	result.OffsetFromPlayer = { 0.0f, 4.65f, -6.79f };

	return result;
}

void Camera::Update(ControllerInput* input)
{
	if (p_Player)
	{
		glm::vec3* playerPos = p_Player->Trans()->Position();
		Position = *playerPos + OffsetFromPlayer;
		
		View = glm::lookAt(Position, *playerPos, Up);

		FreezeMovement = true;
	}
	FpsUpdate(input);
	
}

void Camera::FpsUpdate(ControllerInput* input)
{
	float speed = MoveSpeed * input->DeltaTime;

	float xOffset, yOffset;
	if (input->IsAnalogue)
	{
		xOffset = input->CameraAxisX * StickSensitivity * input->DeltaTime;
		yOffset = input->CameraAxisY * StickSensitivity * input->DeltaTime;
	}
	else
	{
		xOffset = (input->CameraAxisX - input->LastInput->CameraAxisX) * MouseSensitivity;
		yOffset = (input->LastInput->CameraAxisY - input->CameraAxisY) * MouseSensitivity;
	}

	Yaw += xOffset;
	Pitch = glm::clamp(Pitch + yOffset, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);

	Forward = {};
	Forward.x = glm::cos(Yaw) * glm::cos(Pitch);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::sin(Yaw) * glm::cos(Pitch);
	Forward = glm::normalize(Forward);

	Right = glm::normalize(glm::cross(Forward, Up));

	if (input->X && !input->LastInput->X)
	{
		FreezeMovement = !FreezeMovement;
	}

	if (input->IsAnalogue)
	{
		glm::vec3 movementDir = input->MovementAxisX * Right + input->MovementAxisY * Forward;
		if (!FreezeMovement)
		{
			Position += movementDir * speed;
		}
	}
	else
	{
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
	}
	if (input->LeftShoulder)
	{
		Position -= speed * Up;
	}
	if (input->RightShoulder)
	{
		Position += speed * Up;
	}

	View = glm::lookAt(Position, Position + Forward, Up);
}