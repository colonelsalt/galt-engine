#include "AnimationClip.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static AnimationClip* ParseAnimationClip(char* animationFileName)
{
	// TODO: Automatic file path
	char filePath[PATH_MAX];
	CatStr(MODELS_DIRECTORY, animationFileName, filePath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
	Assert(scene && scene->mRootNode && scene->mNumAnimations > 0);

	AnimationClip* animationClip = (AnimationClip*)
		g_Memory->TempAlloc(sizeof(AnimationClip));

	// TODO: Any use case where we don't just want to grab the first animation?
	aiAnimation* aiAnim = scene->mAnimations[0];

	animationClip->LocalDuration = (float)aiAnim->mDuration;
	animationClip->LocalTicksPerSecond = (float)aiAnim->mTicksPerSecond;

	animationClip->NumBoneClips = aiAnim->mNumChannels;

	for (uint32_t i = 0; i < aiAnim->mNumChannels; i++)
	{
		aiNodeAnim* channel = aiAnim->mChannels[i];
		char* boneName = channel->mNodeName.data;
		Entity boneEntity = g_EntityMaster->GetEntityByName(boneName);

		BoneClip* boneClip = (BoneClip*)g_Memory->TempAlloc(sizeof(BoneClip));
		animationClip->ap_BoneClips[boneEntity] = boneClip;
		boneClip->EntityHandle = boneEntity;

		// Parse position keyframes
		boneClip->NumPositionKeys = channel->mNumPositionKeys;
		boneClip->a_PosKeyframes = (PositionKeyframe*)
			g_Memory->TempAlloc(boneClip->NumPositionKeys * sizeof(PositionKeyframe));
		for (uint32_t j = 0; j < channel->mNumPositionKeys; j++)
		{
			PositionKeyframe* key = &boneClip->a_PosKeyframes[j];
			key->Position = *(glm::vec3*)&channel->mPositionKeys[j].mValue;
			key->Timestamp = (float)channel->mPositionKeys[j].mTime;
		}

		// Parse rotation keyframes
		boneClip->NumRotationKeys = channel->mNumRotationKeys;
		boneClip->a_RotKeyframes = (RotationKeyframe*)
			g_Memory->TempAlloc(boneClip->NumRotationKeys * sizeof(RotationKeyframe));
		for (uint32_t j = 0; j < channel->mNumRotationKeys; j++)
		{
			RotationKeyframe* key = &boneClip->a_RotKeyframes[j];
			aiQuaternion* rot = &channel->mRotationKeys[j].mValue;
			key->Rotation = { rot->w, rot->x, rot->y, rot->z };
			key->Timestamp = (float)channel->mRotationKeys[j].mTime;
		}

		// Parse scale keyframes
		boneClip->NumScaleKeys = channel->mNumScalingKeys;
		boneClip->a_ScaleKeyframes = (ScaleKeyframe*)
			g_Memory->TempAlloc(boneClip->NumScaleKeys * sizeof(ScaleKeyframe));
		for (uint32_t j = 0; j < channel->mNumScalingKeys; j++)
		{
			ScaleKeyframe* key = &boneClip->a_ScaleKeyframes[j];
			key->Scale = *(glm::vec3*)&channel->mScalingKeys[j].mValue;
			key->Timestamp = (float)channel->mScalingKeys[j].mTime;
		}

	}
	return animationClip;
}



template <typename T>
static uint32_t GetKeyIndex(T* keys, uint32_t numkeys, float animationTime)
{
	for (uint32_t i = 0; i < numkeys - 1; i++)
	{
		if (animationTime < keys[i + 1].Timestamp)
		{
			return i;
		}
	}
	return numkeys - 2;
}

static float GetLerpParam(float prevKeyTime, float nextKeyTime, float currentTime)
{
	return (currentTime - prevKeyTime) / (nextKeyTime - prevKeyTime);
}

static void BlendPoses(BonePose** blendedPoses,
                       BonePose** sourcePoses,
                       BonePose** targetPoses,
                       float t)
{
	for (uint32_t i = 0; i < MAX_ENTITIES; i++)
	{
		BonePose* sourcePose = sourcePoses[i];
		BonePose* targetPose = targetPoses[i];
		BonePose* blendedPose = blendedPoses[i];
		
		// Either all three exist or none of them do
		if (!sourcePose || !targetPose || !blendedPose)
		{
			Assert(!sourcePose && !targetPose && !blendedPose);
			continue;
		}
		Assert(sourcePose && targetPose && blendedPose);

		blendedPose->Translation = glm::mix(sourcePose->Translation, targetPose->Translation, t);
		blendedPose->Rotation = glm::slerp(sourcePose->Rotation, targetPose->Rotation, t);
		blendedPose->Scale = glm::mix(sourcePose->Scale, targetPose->Scale, t);
	}
}

void AnimationClip::Update(float animationTime)
{
	for (uint32_t i = 0; i < MAX_ENTITIES; i++)
	{
		BoneClip* boneClip = ap_BoneClips[i];
		if (boneClip)
		{
			boneClip->Update(animationTime);
			ap_BonePoses[i] = &boneClip->LocalPose;
		}
		if (p_SourceClip && p_TargetClip)
		{
			p_SourceClip->Update(animationTime);
			p_TargetClip->Update(animationTime);

			BlendPoses(ap_BonePoses,
			           p_SourceClip->ap_BonePoses,
			           p_TargetClip->ap_BonePoses,
			           TargetWeight);
		}

	}
}

void BoneClip::Update(float animationTime)
{
	LocalPose.Translation = InterpolatePosition(animationTime);
	LocalPose.Rotation = InterpolateRotation(animationTime);
	LocalPose.Scale = InterpolateScale(animationTime);
}

glm::vec3 BoneClip::InterpolatePosition(float animationTime)
{
	if (NumPositionKeys == 1)
	{
		return a_PosKeyframes[0].Position;
	}

	uint32_t currentKeyIndex = GetKeyIndex<PositionKeyframe>(a_PosKeyframes,
		NumPositionKeys, animationTime);
	
	PositionKeyframe* currentkey = &a_PosKeyframes[currentKeyIndex];
	PositionKeyframe* nextKey = &a_PosKeyframes[currentKeyIndex + 1];

	float t = GetLerpParam(currentkey->Timestamp, nextKey->Timestamp, animationTime);
	return glm::mix(currentkey->Position, nextKey->Position, t);
}

glm::quat BoneClip::InterpolateRotation(float animationTime)
{
	if (NumRotationKeys == 1)
	{
		return a_ScaleKeyframes[0].Scale;
	}

	uint32_t currentKeyIndex = GetKeyIndex<RotationKeyframe>(a_RotKeyframes,
		NumRotationKeys, animationTime);
	
	RotationKeyframe* currentkey = &a_RotKeyframes[currentKeyIndex];
	RotationKeyframe* nextKey = &a_RotKeyframes[currentKeyIndex + 1];

	float t = GetLerpParam(currentkey->Timestamp, nextKey->Timestamp, animationTime);
	glm::quat interpolatedRot = glm::slerp(currentkey->Rotation, nextKey->Rotation, t);
	return glm::normalize(interpolatedRot);
}

glm::vec3 BoneClip::InterpolateScale(float animationTime)
{
	if (NumScaleKeys == 1)
	{
		return a_ScaleKeyframes[0].Scale;
	}

	uint32_t currentKeyIndex = GetKeyIndex<ScaleKeyframe>(a_ScaleKeyframes,
		NumScaleKeys, animationTime);
	
	ScaleKeyframe* currentkey = &a_ScaleKeyframes[currentKeyIndex];
	ScaleKeyframe* nextKey = &a_ScaleKeyframes[currentKeyIndex + 1];

	float t = GetLerpParam(currentkey->Timestamp, nextKey->Timestamp, animationTime);
	return glm::mix(currentkey->Scale, nextKey->Scale, t);
}