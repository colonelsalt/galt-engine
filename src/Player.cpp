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

void Player::Update(ControllerInput* input, Camera* camera)
{
	constexpr glm::vec3 UP = { 0.0f, 1.0f, 0.0f };

	WalkSpeed = 30.0f;
	JumpHeight = 30.0f;
	Gravity = 120.0f;

	float speed = 0.0f;
	if (input->IsAnalogue && CanMove)
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
		if (runPercentage > 0)
		{
			p_Animator->SetTrigger(PlayerAnimTriggers::MOVE);
		}
		else
		{
			p_Animator->SetTrigger(PlayerAnimTriggers::IDLE);
		}
		p_Animator->SetVar(PlayerAnimVars::MOVE_SPEED, runPercentage);
		speed = runPercentage * WalkSpeed;
	}
	else
	{
		p_Animator->SetTrigger(PlayerAnimTriggers::IDLE);
		// TODO
	}

	if (input->X && !input->LastInput->X)
	{
		CanMove = !CanMove;
	}

	if (input->A && IsGrounded && CanMove)
	{
		// Jump
		p_Animator->SetTrigger(PlayerAnimTriggers::JUMP);
		VerticalSpeed = sqrtf(JumpHeight * Gravity);
		IsGrounded = false;
	}
	if (!IsGrounded)
	{
		VerticalSpeed -= Gravity * input->DeltaTime;
	}

	glm::vec3* position = Trans()->Position();
	*position += (speed * MovementDirection
		+ glm::vec3(0.0f, VerticalSpeed, 0.0f)) * input->DeltaTime;
	
	if (position->y < GroundLevel)
	{
		if (speed > 0)
		{
			p_Animator->SetTrigger(PlayerAnimTriggers::ROLL);
		}
		else
		{
			p_Animator->SetTrigger(PlayerAnimTriggers::LAND);
		}

		position->y = GroundLevel;
		VerticalSpeed = 0.0f;
		IsGrounded = true;
	}

	//*position = { 0.0f, 2.0f, 0.0f };
}