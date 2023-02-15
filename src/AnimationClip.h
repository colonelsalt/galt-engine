#pragma once

#include <glm/gtc/quaternion.hpp>

// Transform of bone relative to its parent
struct BonePose
{
	glm::vec3 Translation;
	glm::quat Rotation;
	glm::vec3 Scale;
};

struct PositionKeyframe
{
	glm::vec3 Position;
	float Timestamp;
};

struct RotationKeyframe
{
	glm::quat Rotation;
	float Timestamp;
};

struct ScaleKeyframe
{
	glm::vec3 Scale;
	float Timestamp;
};

struct TranslationConstraints
{
	bool X;
	bool Y;
	bool Z;
};

// Controls the movement of a single bone within a larger animation clip
struct BoneClip
{
	uint32_t NumPositionKeys;
	PositionKeyframe* a_PosKeyframes;

	uint32_t NumRotationKeys;
	RotationKeyframe* a_RotKeyframes;

	uint32_t NumScaleKeys;
	ScaleKeyframe* a_ScaleKeyframes;

	BonePose LocalPose;

	TranslationConstraints Constraints;

	Entity EntityHandle;

	void Update(float animationTime);

	glm::vec3 InterpolatePosition(float animationTime);
	glm::quat InterpolateRotation(float animationTime);
	glm::vec3 InterpolateScale(float animationTime);
};

struct AnimationClip
{
#if GALT_INTERNAL
	char DebugName[MAX_NAME_LENGTH];
#endif

	float LocalDuration;
	float LocalTicksPerSecond;

	TranslationConstraints Constraints;
	
	BonePose* ap_BonePoses[MAX_ENTITIES];

	// Only for use when playing a single clip ---
	uint32_t NumBoneClips; // Probably unneeded since bone array has constant size
	BoneClip* ap_BoneClips[MAX_ENTITIES];
	// ---

	// Only for use when blending between two clips in the same state ---
	AnimationClip* p_SourceClip;
	AnimationClip* p_TargetClip;
	float TargetWeight;
	float TpsScale;
	inline void SetTargetWeight(float targetWeight)
	{
		TargetWeight = targetWeight - 0.2f;
		float tps = TpsScale * p_SourceClip->LocalDuration;
		LocalTicksPerSecond = glm::mix(10.0f / p_SourceClip->LocalDuration,
		                               30.0f / p_TargetClip->LocalDuration,
		                               targetWeight);
	}
	// ---

	void Update(float animationTime);
	BonePose* GetBonePoses();

	inline void SetConstraints(const TranslationConstraints& constraints)
	{
		for (uint32_t i = 0; i < MAX_MESHES; i++)
		{
			BoneClip* boneClip = ap_BoneClips[i];
			if (boneClip)
			{
				boneClip->Constraints = constraints;
			}
		}
	}
};