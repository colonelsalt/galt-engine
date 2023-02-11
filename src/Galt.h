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

	Camera FpsCamera;
	Entity Plane;
	Entity Cube;
	Entity Lamp;

	Entity PointLight;

	union
	{
		Shader Shaders[3];
		struct
		{
			Shader PrimitiveShader;
			Shader BasicPhongShader;
			Shader FlatColourShader;
		};
	};
};

inline int RoundToInt(float f)
{
	float x = f * 10.0f;
	return (int)x / 10;
}

inline uint32_t SafeTruncateUInt64(uint64_t value)
{
	Assert(value <= 0xFFFFFFFF);
	return (uint32_t)value;
}

inline void Substring(const char* s, int start, int end, char* dest)
{
	for (int i = start; i < end; i++)
	{
		*dest++ = s[i];
	}
	*dest = 0;
}

inline size_t StrLen(const char* s)
{
	int length = 0;
	while (s[length++]);
	return length;
}

inline int LastIndexOf(const char* s, char c)
{
	int lastIndex = -1;
	for (int i = 0; s[i]; i++)
	{
		if (s[i] == c)
		{
			lastIndex = i;
		}
	}
	return lastIndex;
}

inline void CatStr(const char* s1, const char* s2, char* dest)
{
	int resultIndex = 0;
	for (int i = 0; s1[i]; i++)
	{
		dest[resultIndex++] = s1[i];
	}
	for (int i = 0; s2[i]; i++)
	{
		dest[resultIndex++] = s2[i];
	}
	dest[resultIndex] = 0;
}

typedef void GAME_API GameUpdateFunc(GameMemory*, ControllerInput*);