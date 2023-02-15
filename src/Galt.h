#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shader.h"
#include "Primitives.h"
#include "Camera.h"
#include "Mesh.h"
#include "Entity.h"
#include "EntityMaster.h"
#include "AnimationClip.h"

struct FileResult
{
	uint32_t FileSize;
	void* Contents;
};

typedef FileResult ReadFileFunc(const char* fileName);
typedef void FreeFileMemoryFunc(void* memory);
typedef int64_t GetLastWriteTimeFunc(char* fileName);

struct GameMemory
{
	bool IsInitialised;
	bool OpenGlInitialised;
	bool PendingScreenResize;
	int FrameCounter;

	uint64_t PermStorageSize;
	void* PermStorage;

	uint64_t TempStorageSize;
	void* TempStorage;
	uint64_t TempStorageCursor;

	ReadFileFunc* ReadFile;
	FreeFileMemoryFunc* FreeFile;
	GLADloadproc GladLoader;
	GetLastWriteTimeFunc* GetLastWriteTime;

	int ScreenWidth;
	int ScreenHeight;

	inline void* TempAlloc(size_t numBytes)
	{
		void* result = (uint8_t*)TempStorage + TempStorageCursor;
		TempStorageCursor += numBytes;
		Assert(TempStorageCursor < TempStorageSize);
		return result;
	}
};

struct GameState
{
	EntityMaster EntityMasterInstance;

	uint32_t ProjViewUniformBuffer;
	uint32_t SkinningMatricesUniformBuffer;
	
	uint32_t ShadowMapFramebuffer;
	uint32_t ShadowMapTextureId;

	Camera FpsCamera;
	Entity Plane;
	Entity Cube;

	Entity Player;

	Entity PointLight;
	Entity Skybox;

	union
	{
		Shader Shaders[8];
		struct
		{
			Shader PrimitiveShader;
			Shader BasicPhongShader;
			Shader PrimitiveShadowShader;
			Shader MeshShader;
			Shader MeshShadowShader;
			Shader AnimShader;
			Shader SkyboxShader;
			Shader FlatColourShader;
		};
	};
};

typedef void GAME_API GameUpdateFunc(GameMemory*, ControllerInput*);