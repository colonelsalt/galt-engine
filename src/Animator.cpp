#include "Animator.h"

#include <glm/gtx/quaternion.hpp>

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

void Animator::Init(AnimationClip* clip)
{
	p_Clip = clip;
	
	NameTag* name = GetComponent<NameTag>();

	p_Skeleton = FindSkeletonInHierarchy(Trans());
	Assert(p_Skeleton); // Couldn't find this entity's skeleton
}

void Animator::Update(float deltaTime)
{
	AnimationTime += deltaTime * p_Clip->LocalTicksPerSecond;
	if (p_Clip->ShouldLoop)
	{
		AnimationTime = fmod(AnimationTime, p_Clip->LocalDuration);
	}

	p_Clip->Update(AnimationTime);

	UpdateSkinningMatrices(Trans(), glm::mat4(1.0f));
}

void Animator::UpdateSkinningMatrices(Transform* transform,
                                      const glm::mat4& parentTransform)
{
	glm::mat4 localTransform = transform->Local;

	Entity entity = transform->ThisEntity;
	if (p_Clip->ap_BoneClips[entity])
	{
		// If bone is animated, use its animated transform
		BonePose localPose = p_Clip->ap_BoneClips[entity]->LocalPose;

		localTransform = glm::translate(glm::mat4(1.0f), localPose.Translation)
			* glm::toMat4(localPose.Rotation)
			* glm::scale(glm::mat4(1.0f), localPose.Scale);
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
		UpdateSkinningMatrices(transform->a_Children[i], modelSpaceTransform);
	}

}