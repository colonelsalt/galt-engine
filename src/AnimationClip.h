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

	Entity EntityHandle;

	void Update(float animationTime);

	glm::vec3 InterpolatePosition(float animationTime);
	glm::quat InterpolateRotation(float animationTime);
	glm::vec3 InterpolateScale(float animationTime);
};

struct AnimationClip
{
	float LocalDuration;
	float LocalTicksPerSecond;
	bool ShouldLoop;

	uint32_t NumBoneClips;
	BoneClip* a_BoneClips[MAX_ENTITIES];

	void Update(float animationTime);
};