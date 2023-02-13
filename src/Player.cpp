#include "Player.h"

void Player::Init()
{
	WalkSpeed = 5.0f;
	RunSpeed = 10.0f;
	JumpHeight = 4.0f;
	Gravity = 60.0f;
	IsGrounded = true;
	GroundLevel = Trans()->Position()->y;

	p_Animator = GetComponent<Animator>();

	Assert(p_Animator);
}

static float Max(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	return b;
}

static float Abs(float x)
{
	if (x < 0)
	{
		return -x;
	}
	return x;
}

void Player::Update(ControllerInput* input, Camera* camera)
{
	constexpr glm::vec3 UP = { 0.0f, 1.0f, 0.0f };
	constexpr glm::vec3 FORWARD = { 0.0f, 0.0f, -1.0f };
	constexpr glm::vec3 BACK = { 0.0f, 0.0f, 1.0f };
	constexpr glm::vec3 LEFT = { -1.0f, 0.0f, 0.0f };
	constexpr glm::vec3 RIGHT = { 1.0f, 0.0f, 0.0f };

	float speed = 0.0f;
	if (input->IsAnalogue)
	{
		if (input->MovementAxisX != 0.0f || input->MovementAxisY != 0.0f)
		{
			glm::vec3 forward = camera->Forward * input->MovementAxisY;
			forward.y = 0.0f;
			glm::vec3 right = camera->Right * input->MovementAxisX;
			right.y = 0.0f;
			MovementDirection = glm::normalize(forward + right);
			
			Trans()->SetRotation(atan2f(MovementDirection.x, MovementDirection.z) / 2.0f,
			                     UP);
		}
		speed = Max(Abs(input->MovementAxisX), Abs(input->MovementAxisY)) * WalkSpeed;
	}
	else
	{
		// TODO
	}

	if (input->X && !input->LastInput->X)
	{
		CanMove = !CanMove;
	}

	if (input->A && IsGrounded)
	{
		// Jump
		VerticalSpeed = sqrtf(JumpHeight * Gravity);
		IsGrounded = false;
	}
	if (!IsGrounded)
	{
		VerticalSpeed -= Gravity * input->DeltaTime;
	}

	glm::vec3* position = Trans()->Position();
	if (CanMove)
	{
		*position += (speed * MovementDirection
			+ glm::vec3(0.0f, VerticalSpeed, 0.0f)) * input->DeltaTime;
	}
	if (position->y < GroundLevel)
	{
		position->y = GroundLevel;
		VerticalSpeed = 0.0f;
		IsGrounded = true;
	}

	//*position = { 0.0f, 2.0f, 0.0f };
}