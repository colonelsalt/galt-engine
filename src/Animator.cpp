#include "Animator.h"

#include <glm/gtx/quaternion.hpp>

#include "Util.h"

// Very ugly (and potentially buggy), but it will do for now...
Skeleton* FindSkeletonInHierarchy(Transform* transform)
{
	Mesh* mesh = transform->ThisEntity.GetComponent<Mesh>();
	if (mesh && mesh->p_Skeleton)
	{
		return mesh->p_Skeleton;
	}
	for (uint32_t i = 0; i < transform->NumChildren; i++)
	{
		Skeleton* childSkeleton = FindSkeletonInHierarchy(transform->a_Children[i]);
		if (childSkeleton)
		{
			return childSkeleton;
		}
	}
	return nullptr;
}

void Animator::Init()
{
	p_Skeleton = FindSkeletonInHierarchy(Trans());
	Assert(p_Skeleton); // Couldn't find this entity's skeleton
}

void Animator::Update(float deltaTime)
{
	BonePose** bonePoses = nullptr;
	
	Assert(!p_CurrentState != !p_CurrentTransition); // XOR
	if (p_CurrentState)
	{
		bonePoses = p_CurrentState->p_Clip->ap_BonePoses;
		p_CurrentState->Update(deltaTime);

		float upcomingTransitionDuration = 0.0f;
		if (p_CurrentState->p_OnCompleteTransition)
		{
			upcomingTransitionDuration = p_CurrentState->p_OnCompleteTransition->Duration;
		}

		if (p_CurrentState->CurrentTime >= p_CurrentState->Duration - upcomingTransitionDuration)
		{
			if (p_CurrentState->p_OnCompleteTransition)
			{
				GALT_LOG("%s completed; starting %s transition\n",
				         p_CurrentState->DebugName,
				         p_CurrentState->p_OnCompleteTransition->DebugName);

				p_CurrentTransition = p_CurrentState->p_OnCompleteTransition;
				p_CurrentTransition->p_TargetState->CurrentTime = 0.0f;
				p_CurrentState = nullptr;
			}
		}
	}
	else if (p_CurrentTransition)
	{
		bonePoses = p_CurrentTransition->ap_BonePoses;
		p_CurrentTransition->Update(deltaTime);
		if (p_CurrentTransition->TimePassed >= p_CurrentTransition->Duration)
		{
			GALT_LOG("%s transition completed; entering %s\n",
			         p_CurrentTransition->DebugName,
			         p_CurrentTransition->p_TargetState->DebugName);

			p_CurrentTransition->TimePassed = 0.0f;
			p_CurrentState = p_CurrentTransition->p_TargetState;
			p_CurrentTransition = nullptr;
		}

	}
	Assert(bonePoses);

	UpdateSkinningMatrices(Trans(), bonePoses, glm::mat4(1.0f));
}

void Animator::UpdateSkinningMatrices(Transform* transform,
                                      BonePose** bonePoses,
                                      const glm::mat4& parentTransform)
{
	glm::mat4 localTransform = transform->Local;

	Entity entity = transform->ThisEntity;
	if (bonePoses[entity])
	{
		// If bone is animated, use its animated transform
		BonePose* localPose = bonePoses[entity];

		localTransform = glm::translate(glm::mat4(1.0f), localPose->Translation)
			* glm::toMat4(localPose->Rotation)
			* glm::scale(glm::mat4(1.0f), localPose->Scale);
	}

	glm::mat4 modelSpaceTransform = parentTransform * localTransform;
	
	int boneId = p_Skeleton->EntityToBoneId[entity];
	Assert(boneId < MAX_TOTAL_BONES);
	if (boneId > -1)
	{
		// If bone is bound to a vertex, update its skinning matrix
		Bone* bone = &p_Skeleton->Bones[boneId];
		SkinningMatrices[boneId] = modelSpaceTransform * bone->InverseBindPose;
	}

	for (uint32_t i = 0; i < transform->NumChildren; i++)
	{
		UpdateSkinningMatrices(transform->a_Children[i], bonePoses, modelSpaceTransform);
	}
}

void Animator::SetTrigger(int triggerId)
{
	if (p_CurrentState)
	{
		Assert(triggerId < MAX_TRIGGERS);
		Transition* transition = p_CurrentState->ap_OnTriggerTransitions[triggerId];
		if (transition)
		{
			GALT_LOG("Received trigger; starting %s\n", transition->DebugName);

			p_CurrentState = nullptr;
			p_CurrentTransition = transition;

			if (!transition->p_TargetState->ShouldLoop)
			{
				transition->p_TargetState->CurrentTime = 0.0f;
			}
		}

	}
}

void Animator::SetVar(int varId, float value)
{
	if (p_CurrentState)
	{
		if (p_CurrentState->BlendVariable.VarId == varId)
		{
			p_CurrentState->BlendVariable.Value = value;
		}
	}
	else if (p_CurrentTransition)
	{
		if (p_CurrentTransition->p_TargetState->BlendVariable.VarId == varId)
		{
			p_CurrentTransition->p_TargetState->BlendVariable.Value = value;
		}
	}
}