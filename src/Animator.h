#pragma once

#include "AnimationStates.h"

struct Animator : public Component
{
	glm::mat4 SkinningMatrices[MAX_TOTAL_BONES];

	float AnimationTime;

	AnimationState* p_CurrentState;
	Transition* p_CurrentTransition;

	Skeleton* p_Skeleton;

	COMPONENT_DEF(ComponentType::ANIMATOR, MAX_ANIMATORS);

	void Init();

	void Update(float deltaTime);
	void UpdateSkinningMatrices(Transform* transform,
	                            BonePose** bonePoses,
	                            const glm::mat4& parentTransform);
	void SetTrigger(int triggerId);
	void SetVar(int varId, float value);
};
