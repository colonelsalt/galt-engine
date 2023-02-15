#pragma once

#include "AnimationClip.h"

constexpr uint32_t MAX_TRIGGERS = 5;

struct Transition;

struct AnimationVar
{
	int VarId;
	float Value;
};

struct AnimationState
{
#if GALT_INTERNAL
	char DebugName[MAX_NAME_LENGTH];
#endif

	AnimationClip* p_Clip;

	Transition* p_OnCompleteTransition;
	Transition* ap_OnTriggerTransitions[MAX_TRIGGERS];

	AnimationVar BlendVariable;

	bool ShouldLoop;
	float Duration;
	float CurrentTime;

	void Update(float deltaTime);
};

struct Transition
{
#if GALT_INTERNAL
	char DebugName[MAX_NAME_LENGTH];
#endif

	AnimationState* p_SourceState;
	AnimationState* p_TargetState;

	BonePose* ap_BonePoses[MAX_ENTITIES];

	float Duration;
	float TimePassed;

	void Update(float deltaTime);
};