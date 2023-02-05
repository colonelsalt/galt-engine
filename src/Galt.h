#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shader.h"
#include "Primitives.h"
#include "Camera.h"

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

	inline void ResetTempMemory()
	{
		TempStorageCursor = 0;
	}

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
	uint32_t TestVertexArrayId;

	Camera FpsCamera;
	Primitive Plane;
	Primitive Cube;

	union
	{
		Shader Shaders[1];
		struct
		{
			Shader PrimitiveShader;
		};
	};
};

inline uint32_t SafeTruncateUInt64(uint64_t value)
{
	Assert(value <= 0xFFFFFFFF);
	return (uint32_t)value;
}

inline size_t StrLen(const char* s)
{
	int length = 0;
	while (s[length++]);
	return length;
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

inline static char* CatStrTemp(const char* s1, const char* s2, GameMemory* memory)
{
	size_t length = StrLen(s1) + StrLen(s2) + 1;
	char* result = (char*)memory->TempAlloc(length);
	int resultIndex = 0;
	for (int i = 0; s1[i]; i++)
	{
		result[resultIndex++] = s1[i];
	}
	for (int i = 0; s2[i]; i++)
	{
		result[resultIndex++] = s2[i];
	}
	result[resultIndex] = 0;
	return result;
}

typedef void GAME_API GameUpdateFunc(GameMemory*, ControllerInput*);