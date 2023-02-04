#pragma once

#include <stdint.h>
#include <glad/glad.h>

#include "Shader.h"

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)
#define Assert(x) { if (!(x)) __debugbreak(); }

#define GAME_API __declspec(dllexport)

struct FileResult
{
	uint32_t FileSize;
	void* Contents;
};

typedef FileResult ReadFileFunction(const char* fileName);
typedef void FreeFileMemoryFunction(void* memory);

struct ControllerInput
{
	bool IsConnected;
	bool IsAnalogue;

	union
	{
		bool Buttons[10];
		struct
		{
			bool Up;
			bool Down;
			bool Left;
			bool Right;

			bool A;
			bool B;
			bool X;
			bool Y;

			bool Start;
			bool Back;
		};
	};
};


struct GameMemory
{
	bool IsInitialised;
	bool OpenGlInitialised;

	uint64_t PermStorageSize;
	void* PermStorage;

	uint64_t TempStorageSize;
	void* TempStorage;
	uint64_t TempStorageCursor;

	ReadFileFunction* ReadFile;
	FreeFileMemoryFunction* FreeFile;
	GLADloadproc GladLoader;

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
	uint32_t VertexArrayId;

	float PlayerX;
	float PlayerY;

	Shader TestShader;
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

inline static char* CatStr(const char* s1, const char* s2, GameMemory* memory)
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