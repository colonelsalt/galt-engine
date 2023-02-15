#include "AnimationStates.h"

#include "Util.h"

static AnimationState* CreateAnimationState(AnimationClip* clip, char* debugName = nullptr)
{
	AnimationState* result = (AnimationState*)g_Memory->TempAlloc(sizeof(AnimationState));
	result->p_Clip = clip;
	result->Duration = clip->LocalDuration;

#if GALT_INTERNAL
	StrCopy(debugName, result->DebugName);
#endif

	return result;
}

static Transition* CreateTransition(AnimationState* sourceState,
                                    AnimationState* targetState,
                                    float duration,
                                    char* debugName = nullptr)
{
	Transition* result = (Transition*)g_Memory->TempAlloc(sizeof(Transition));
	result->p_SourceState = sourceState;
	result->p_TargetState = targetState;
	result->Duration = duration;
	
	// Alocate space for the blended poses
	for (uint32_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (sourceState->p_Clip->ap_BoneClips[i] || targetState->p_Clip->ap_BoneClips[i])
		{
			result->ap_BonePoses[i] = (BonePose*)g_Memory->TempAlloc(sizeof(BonePose));
		}
	}

#if GALT_INTERNAL
	StrCopy(debugName, result->DebugName);
#endif
	return result;
}

void AnimationState::Update(float deltaTime)
{
	Assert(p_Clip);
	CurrentTime += deltaTime * p_Clip->LocalTicksPerSecond;
	if (CurrentTime >= Duration)
	{
		if (ShouldLoop)
		{
			GALT_LOG("%s looped.\n", DebugName);
			CurrentTime = fmod(CurrentTime, p_Clip->LocalDuration);
		}
		else
		{
			GALT_LOG("%s halted on final frame.\n", DebugName);
			CurrentTime = Duration;
		}
	}
	if (BlendVariable.VarId)
	{
		p_Clip->SetTargetWeight(BlendVariable.Value);
	}
	p_Clip->Update(CurrentTime);
}

void Transition::Update(float deltaTime)
{
	Assert(p_SourceState && p_TargetState);
	Assert(p_SourceState->p_Clip && p_TargetState->p_Clip);
	
	TimePassed += deltaTime;
	p_SourceState->Update(deltaTime);
	p_TargetState->Update(deltaTime);

	BlendPoses(ap_BonePoses,
	           p_SourceState->p_Clip->ap_BonePoses,
	           p_TargetState->p_Clip->ap_BonePoses,
	           TimePassed / Duration);
}