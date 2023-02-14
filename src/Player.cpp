#include "Player.h"

enum PlayerAnimationTriggers
{
	LOCOMOTION,
	JUMP,
	IDLE
};

enum PlayerAnimationBlends
{
	WALK,
	RUN
};

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

void Player::Update(ControllerInput* input, Camera* camera)
{
	constexpr glm::vec3 UP = { 0.0f, 1.0f, 0.0f };
	constexpr glm::vec3 FORWARD = { 0.0f, 0.0f, -1.0f };
	constexpr glm::vec3 BACK = { 0.0f, 0.0f, 1.0f };
	constexpr glm::vec3 LEFT = { -1.0f, 0.0f, 0.0f };
	constexpr glm::vec3 RIGHT = { 1.0f, 0.0f, 0.0f };

	WalkSpeed = 20.0f;
	JumpHeight = 20.0f;
	Gravity = 100.0f;

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
			
			float yRotation = atan2f(MovementDirection.x, MovementDirection.z);
			Trans()->SetRotation(yRotation, UP);
		}

		float runPercentage = Max(Abs(input->MovementAxisX), Abs(input->MovementAxisY));
		speed = runPercentage * WalkSpeed;
		if (speed > 0)
		{
			//p_Animator->SetTrigger(PlayerAnimationTriggers::LOCOMOTION);

		}
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