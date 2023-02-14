#pragma once

#include "AnimationClip.h"

constexpr uint32_t MAX_TRANSITIONS = 3;

struct Transition;

struct AnimationState
{
	AnimationClip* p_Clip;

	Transition* ap_OnTriggerTransitions[MAX_TRANSITIONS];
	Transition* p_OnCompleteTransition;

	bool ShouldLoop;
	float CompletionTime;
};

struct Transition
{
	AnimationState* p_SourceState;
	AnimationState* p_TargetState;

	BonePose* ap_BonePoses[MAX_ENTITIES];

	float Duration;
	float TimePassed;
};

struct Animator : public Component
{
	glm::mat4 SkinningMatrices[MAX_TOTAL_BONES];

	float AnimationTime;

	AnimationClip* p_Clip;
	Skeleton* p_Skeleton;

	COMPONENT_DEF(ComponentType::ANIMATOR, MAX_ANIMATORS);

	void Init(AnimationClip* clip);

	void Update(float deltaTime);
	void UpdateSkinningMatrices(Transform* transform, const glm::mat4& parentTransform);
};
