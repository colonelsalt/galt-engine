#pragma once

#include <stdint.h>

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)
#define Assert(x) { if (!(x)) __debugbreak(); }

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

	uint64_t PermStorageSize;
	void* PermStorage;
};

struct GameState
{
	uint32_t VertexArrayId;

	float PlayerX;
	float PlayerY;

	float ClearR;
	float ClearG;
	float ClearB;
};

void UpdateAndRender(GameMemory* memory, ControllerInput* input);