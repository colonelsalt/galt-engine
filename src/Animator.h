#pragma once

#include "AnimationClip.h"

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

struct AnimationState
{
	bool ShouldLoop;
	float CompletionTime;
};