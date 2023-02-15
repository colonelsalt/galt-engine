#pragma once

struct Camera;
struct Animator;

enum PlayerAnimTriggers
{
	MOVE = 0,
	IDLE,
	JUMP,
	LAND,
	ROLL
};

enum PlayerAnimVars
{
	MOVE_SPEED = 1
};

struct Player : Component
{
	float WalkSpeed;
	float RunSpeed;
	float JumpHeight;
	float Gravity;

	float VerticalSpeed;
	bool IsGrounded;
	float GroundLevel;
	bool CanMove;

	glm::vec3 MovementDirection;

	Animator* p_Animator;
	
	COMPONENT_DEF(ComponentType::PLAYER, MAX_PLAYERS);

	void Init();
	void Update(ControllerInput* input, Camera* camera);
};